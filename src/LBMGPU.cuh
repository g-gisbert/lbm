#ifndef EFS_LBMGPU_H
#define EFS_LBMGPU_H

#include <cuda_runtime.h>
#include <cstdint>

struct SimulationParams {
    float invTau;
    float speed;
    bool mode;
};

class LBMGPU {
public:
    LBMGPU(int w, int h);
    ~LBMGPU();

    void step(SimulationParams& params, unsigned char* buffer);

private:


    int m_width;
    int m_height;

    dim3 m_blockSize;
    dim3 m_gridSize;

    float* d_oldDensities; // w*h*9
    float* d_densities; // w*h*9
    float* d_velU; // w*h
    float* d_velV; // w*h
    uint8_t* d_walls; // w*h
    float* d_oldInk; // w*h*3
    float* d_ink; // w*h*3
    float* h_velU; // w*h
    float* h_velV; // w*h
    float* h_ink; // w*h*3
    const float h_weights[9] = {4.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0};
    const float h_e[18] = {0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f,  0.0f, -1.0f,  -1.0f, 0.0f,
                     1.0, 1.0f,  1.0f, -1.0f,  -1.0f, -1.0f,  -1.0f, 1.0f};

};

__global__ void initializeWalls(unsigned char* array, unsigned int rows, unsigned int cols);
__global__ void initializeDensities(float* d_densities, unsigned int rows, unsigned int cols);
void swapPointers(float*& array1, float*& array2);
__global__ void addInk(float* d_ink, unsigned int rows, unsigned int cols);
__global__ void advection(float* d_densities, const float* d_oldDensities, unsigned int rows, unsigned int cols);
__global__ void collision(float invTau, float speed, float* d_densities, float* d_velU, float* d_velV,
                          unsigned char* d_walls, float* d_ink, float* d_oldInk, unsigned int rows, unsigned int cols);
__device__ float kerPositiveFmod(float val, float mod);

#endif //EFS_LBMGPU_H

