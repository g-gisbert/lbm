#include "LBMGPU.cuh"
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <chrono>



__constant__ float d_weights[9], d_e[18];

LBMGPU::LBMGPU(int w, int h) :
    m_width(w), m_height(h), m_blockSize(dim3{16,16}),
    m_gridSize(dim3{(w + m_blockSize.x - 1) / m_blockSize.x, (h + m_blockSize.y - 1) / m_blockSize.y}),
    h_velU(new float[m_width * m_height]), h_velV(new float[m_width * m_height]), h_ink(new float[m_width * m_height * 3]) {

    cudaMalloc((void**)&d_oldDensities, w * h * 9 * sizeof(float));
    cudaMalloc((void**)&d_densities, w * h * 9 * sizeof(float));
    cudaMalloc((void**)&d_velU, w * h * sizeof(float));
    cudaMalloc((void**)&d_velV, w * h * sizeof(float));
    cudaMalloc((void**)&d_walls, w * h * sizeof(uint8_t));
    cudaMalloc((void**)&d_ink, 3 * w * h * sizeof(float));
    cudaMalloc((void**)&d_oldInk, 3 * w * h * sizeof(float));

    cudaMemset(d_oldDensities, 0, w * h * 9 * sizeof(float));
    cudaMemset(d_densities, 0, w * h * 9 * sizeof(float));
    cudaMemset(d_velU, 0, w * h * sizeof(float));
    cudaMemset(d_velV, 0, w * h * sizeof(float));
    cudaMemset(d_ink, 0, 3 * w * h * sizeof(float));
    cudaMemset(d_oldInk, 0, 3 * w * h * sizeof(float));

    cudaMemcpyToSymbol(d_weights, h_weights, 9 * sizeof(float));
    cudaMemcpyToSymbol(d_e, h_e, 18 * sizeof(float));

    initializeWalls<<<m_gridSize, m_blockSize>>>(d_walls, h, w);
    cudaDeviceSynchronize();
    initializeDensities<<<m_gridSize, m_blockSize>>>(d_densities, h, w);
    cudaDeviceSynchronize();
}

LBMGPU::~LBMGPU() {
    cudaFree(d_oldDensities);
    cudaFree(d_densities);
    cudaFree(d_velU);
    cudaFree(d_velV);
    cudaFree(d_walls);
    cudaFree(d_ink);
    cudaFree(d_oldInk);
    delete[] h_velU;
    delete[] h_velV;
    delete[] h_ink;
}


void LBMGPU::step(SimulationParams& params, unsigned char* buffer) {

    for (int iter = 0; iter < 10; ++iter) {

        swapPointers(d_oldDensities, d_densities);
        swapPointers(d_oldInk, d_ink);

        addInk<<<m_gridSize, m_blockSize>>>(d_oldInk, m_height, m_width);

        advection<<<m_gridSize, m_blockSize>>>(d_densities, d_oldDensities, m_height, m_width);
        cudaDeviceSynchronize();

        collision<<<m_gridSize, m_blockSize>>>(params.invTau, params.speed, d_densities, d_velU,
                                                               d_velV, d_walls, d_ink, d_oldInk, m_height, m_width);
        cudaDeviceSynchronize();
    }

    cudaMemcpy(h_velU, d_velU, m_width * m_height * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_velV, d_velV, m_width * m_height * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_ink, d_ink, m_width * m_height * 3 * sizeof(float), cudaMemcpyDeviceToHost);

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            float inkR = h_ink[i * m_width * 3 + j * 3 + 0] * 255;
            float inkG = h_ink[i * m_width * 3 + j * 3 + 1] * 255;
            float inkB = h_ink[i * m_width * 3 + j * 3 + 2] * 255;
            float velU = h_velU[i * m_width + j];
            float velV = h_velV[i * m_width + j];
            float value = sqrtf(velU * velU + velV * velV) * 5.0 * 255;

            if (params.mode) {
                buffer[3 * (i * m_width + j)] = value / 4;
                buffer[3 * (i * m_width + j) + 1] = value / 2;
                buffer[3 * (i * m_width + j) + 2] = value;
            }
            else {
                buffer[3 * (i * m_width + j)] = inkR;
                buffer[3 * (i * m_width + j) + 1] = inkG;
                buffer[3 * (i * m_width + j) + 2] = inkB;
            }
        }
    }
}


__global__ void initializeWalls(unsigned char* array, unsigned int rows, unsigned int cols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {
        if ((row - rows/2)*(row - rows/2) + (col - 320)*(col - 320) < 105*105)
            array[row * cols + col] = 255;
        else if ((row - 0.75*rows)*(row - 0.75*rows) + (col - 200)*(col - 200) < 25*25)
            array[row * cols + col] = 255;
        else if ((row - 0.25*rows)*(row - 0.25*rows) + (col - 200)*(col - 200) < 25*25)
            array[row * cols + col] = 255;
        else
            array[row * cols + col] = 0;
    }
}

__global__ void initializeDensities(float* d_densities, unsigned int rows, unsigned int cols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {

        float v =  0.1f * (1.0f + 0.1f * float(row) / float(rows));
        float lastTerm = -1.5f * v*v;
        for (int k = 0; k < 9; ++k) {
            float eiu = d_e[2*k+1] * v;
            float feq = d_weights[k] * (1.0f + 3.0f * eiu + 4.5f * eiu * eiu + lastTerm);
            d_densities[row * cols * 9 + col * 9 + k] = feq;
        }
    }
}

void swapPointers(float*& array1, float*& array2) {
    float *tmp = array1;
    array1 = array2;
    array2 = tmp;
}

__global__ void addInk(float* d_ink, unsigned int rows, unsigned int cols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {
        if (row >= 140+rows/2 && row < 200+rows/2) {
            d_ink[row * cols * 3 + 0 * 3 + 0] = 1.0f;
            d_ink[row * cols * 3 + 0 * 3 + 1] = 0.0f;
            d_ink[row * cols * 3 + 0 * 3 + 2] = 0.2f;
        }
        if (row < rows/2-140 && row >= rows/2-200) {
            d_ink[row * cols * 3 + 0 * 3 + 0] = 0.0f;
            d_ink[row * cols * 3 + 0 * 3 + 1] = 0.2f;
            d_ink[row * cols * 3 + 0 * 3 + 2] = 1.0f;
        }
        if (row >= rows/2-25 && row < 25+rows/2) {
            d_ink[row * cols * 3 + 0 * 3 + 0] = 0.2f;
            d_ink[row * cols * 3 + 0 * 3 + 1] = 1.0f;
            d_ink[row * cols * 3 + 0 * 3 + 2] = 0.2f;
        }
    }
}

__global__ void advection(float* d_densities, const float* d_oldDensities, unsigned int rows, unsigned int cols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {
        unsigned int xplus = ((col == cols - 1) ? (0) : (col + 1));
        unsigned int xminus = ((col == 0) ? (cols - 1) : (col - 1));
        unsigned int yplus = ((row == rows - 1) ? (0) : (row + 1));
        unsigned int yminus = ((row == 0) ? (rows - 1) : (row - 1));
        d_densities[row * cols * 9 + col * 9 + 0] = d_oldDensities[row * cols * 9 + col * 9 + 0];
        d_densities[row * cols * 9 + col * 9 + 1] = d_oldDensities[row * cols * 9 + xminus * 9 + 1];
        d_densities[row * cols * 9 + col * 9 + 2] = d_oldDensities[yminus * cols * 9 + col * 9 + 2];
        d_densities[row * cols * 9 + col * 9 + 3] = d_oldDensities[row * cols * 9 + xplus * 9 + 3];
        d_densities[row * cols * 9 + col * 9 + 4] = d_oldDensities[yplus * cols * 9 + col * 9 + 4];
        d_densities[row * cols * 9 + col * 9 + 5] = d_oldDensities[yminus * cols * 9 + xminus * 9 + 5];
        d_densities[row * cols * 9 + col * 9 + 6] = d_oldDensities[yminus * cols * 9 + xplus * 9 + 6];
        d_densities[row * cols * 9 + col * 9 + 7] = d_oldDensities[yplus * cols * 9 + xplus * 9 + 7];
        d_densities[row * cols * 9 + col * 9 + 8] = d_oldDensities[yplus * cols * 9 + xminus * 9 + 8];
    }
}

__global__ void collision(float invTau, float speed, float* d_densities, float* d_velU, float* d_velV, unsigned char* d_walls,
                          float* d_ink, float* d_oldInk, unsigned int rows, unsigned int cols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {
        // Rho
        float rho = d_densities[row * cols * 9 + col * 9 + 0] + d_densities[row * cols * 9 + col * 9 + 1] +
                    d_densities[row * cols * 9 + col * 9 + 2] + d_densities[row * cols * 9 + col * 9 + 3] +
                    d_densities[row * cols * 9 + col * 9 + 4] + d_densities[row * cols * 9 + col * 9 + 5] +
                    d_densities[row * cols * 9 + col * 9 + 6] + d_densities[row * cols * 9 + col * 9 + 7] +
                    d_densities[row * cols * 9 + col * 9 + 8];

        if (d_walls[row * cols + col] > 127) {
            rho = 1.0f;
        }

        // Update velocities
        float velU = (d_densities[row * cols * 9 + col * 9 + 2] - d_densities[row * cols * 9 + col * 9 + 4] +
                      d_densities[row * cols * 9 + col * 9 + 5] + d_densities[row * cols * 9 + col * 9 + 6] -
                      d_densities[row * cols * 9 + col * 9 + 7] - d_densities[row * cols * 9 + col * 9 + 8]) / rho;
        float velV = (d_densities[row * cols * 9 + col * 9 + 1] - d_densities[row * cols * 9 + col * 9 + 3] +
                      d_densities[row * cols * 9 + col * 9 + 5] - d_densities[row * cols * 9 + col * 9 + 6] -
                      d_densities[row * cols * 9 + col * 9 + 7] + d_densities[row * cols * 9 + col * 9 + 8]) / rho;

        float norm = sqrtf(velU*velU + velV*velV);
        if (norm > 0.2f) {
            velU *= 0.2f / norm;
            velV *= 0.2f / norm;
        }
        if (col == 0 || col == cols-1) {
            rho = 1.0f;
            velU = 0.0f;
            velV = 0.1f;
        }
        if (d_walls[row * cols + col] > 127) {
            velU = 0.0f;
            velV = 0.0f;
        }
        d_velU[row * cols + col] = velU;
        d_velV[row * cols + col] = velV;

        // Ink
        float destRow = float(col) - 1.0f * velV * speed;
        float destColumn = float(row) - 1.0f * velU * speed;
        float wj = destRow - floorf(destRow);
        float wi = destColumn - floorf(destColumn);
        int jDest = kerPositiveFmod(floorf(destRow), (cols - 1));
        int iDest = kerPositiveFmod(floorf(destColumn), (rows - 1));

        float r = (1.0f - wi)*(1.0f - wj)*d_oldInk[iDest * cols * 3 + jDest * 3 + 0] +
                  (1.0f - wi)*wj*d_oldInk[iDest * cols * 3 + (jDest+1) * 3 + 0] +
                  wi*(1.0f - wj)*d_oldInk[(iDest+1) * cols * 3 + jDest * 3 + 0] +
                  wi*wj*d_oldInk[(iDest+1) * cols * 3 + (jDest+1) * 3 + 0];
        float g = (1.0f - wi)*(1.0f - wj)*d_oldInk[iDest * cols * 3 + jDest * 3 + 1] +
                  (1.0f - wi)*wj*d_oldInk[iDest * cols * 3 + (jDest+1) * 3 + 1] +
                  wi*(1.0f - wj)*d_oldInk[(iDest+1) * cols * 3 + jDest * 3 + 1] +
                  wi*wj*d_oldInk[(iDest+1) * cols * 3 + (jDest+1) * 3 + 1];
        float b = (1.0f - wi)*(1.0f - wj)*d_oldInk[iDest * cols * 3 + jDest * 3 + 2] +
                  (1.0f - wi)*wj*d_oldInk[iDest * cols * 3 + (jDest+1) * 3 + 2] +
                  wi*(1.0f - wj)*d_oldInk[(iDest+1) * cols * 3 + jDest * 3 + 2] +
                  wi*wj*d_oldInk[(iDest+1) * cols * 3 + (jDest+1) * 3 + 2];
        if (r > 1.0)
            r = 1.0;
        if (g > 1.0)
            g = 1.0;
        if (b > 1.0)
            b = 1.0;
        d_ink[row * cols * 3 + col * 3 + 0] = r*0.9999f;
        d_ink[row * cols * 3 + col * 3 + 1] = g*0.9999f;
        d_ink[row * cols * 3 + col * 3 + 2] = b*0.9999f;

        // Collision eq
        float lastTerm = - 1.5f*(velU*velU + velV*velV);
        for (int k = 0; k < 9; ++k) {
            float eiu = d_e[2*k]*velU + d_e[2*k+1]*velV;
            float feq = rho * d_weights[k] * (1.0f + 3.0f*eiu + 4.5f*eiu*eiu + lastTerm);
            d_densities[row * cols * 9 + col * 9 + k] += invTau * (feq - d_densities[row * cols * 9 + col * 9 + k]);

            if (d_walls[row * cols + col] > 127 || col == 0 || col == cols-1) {
                d_densities[row * cols * 9 + col * 9 + k] = feq;
            }
        }
    }
}

__device__ float kerPositiveFmod(float val, float mod) {
    float ans = fmodf(val, mod);
    if (ans < 0.0f)
        ans += mod;
    return ans;
}
