#include "LBM.h"

#include <iostream>
#include "vec2.h"
#include <random>

LBM::LBM(int w, int h) : m_width(w), m_height(h), m_invTau(1.95f),
                         m_oldDensities(w, h, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
                         m_densities(w, h, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
                         m_rho(w, h, 0.0f), m_velocities(w, h, Vec2::zero()), m_walls(w, h, false) {

    // Set walls
    for (int i = 0; i < h; ++i) {
        //m_walls(i, 0) = 255;
        //m_walls(i, w-1) = 255;
    }

    for (int j = 0; j < w; ++j) {
        //m_walls(0, j) = 255;
        //m_walls(h-1, j) = 255;
    }

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (norm2(Vec2{float(i)-40, float(j)-50}) < 12*12)
            //    m_walls(i, j) = 255;
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-60, float(j)-80}) < 5*5)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-20, float(j)-80}) < 5*5)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-40, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-50, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-30, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-20, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-10, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-70, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
            if (norm2(Vec2{float(i)-60, float(j)-250}) < 3*3)
                m_walls(i, j) = true;
        }
    }

    // Set random
    /*std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0,1.0);
    for (int i = 1; i < h-1; ++i) {
        for (int j = 1; j < w-1; ++j) {
            for (int k = 0; k < 9; ++k) {
                //if (norm2(Vec2{float(i)-100, float(j)-100}) >= 35*35)
                m_densities(i, j)[k] = 1.0f + 0.01f * distribution(generator);
            }
            m_densities(i, j)[1] = 1.0;
        }
    }*/

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            float rho = 1.0f;
            Vec2 v = Vec2{0.0f, 0.1f*(1.0f+0.1f*float(i)/float(h) )};
            float lastTerm = - 1.5f*dot(v, v);
            for (int k = 0; k < 9; ++k) {

                float eiu = dot(m_e[k], v);
                float feq = rho * m_weights[k] * (1.0f + 3.0f*eiu + 4.5f*eiu*eiu + lastTerm);
                m_densities(i, j)[k] = feq;
            }
        }
    }


}


void LBM::step(float deltaTime, unsigned char* buffer) {

    //externalForces();
    for (int osef = 0; osef < 10; osef++) {
        advection();
        float maxV = collision(deltaTime);
    }

    //float invMax = 1.0f / maxV;
    //std::cout << "LBM" << std::endl;
    // display
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            float value = norm(m_velocities(i, j))*5.0*255;
            float valueX = m_velocities(i, j).x*5.0*127+127*norm(m_velocities(i, j))*5.0;
            float valueY = m_velocities(i, j).y*5.0*127+127*norm(m_velocities(i, j))*5.0;
            //value = m_walls(i, j);

            buffer[3 * (i * m_width + j)] = value;
            buffer[3 * (i * m_width + j) + 1] = valueX;
            buffer[3 * (i * m_width + j) + 2] = valueY;
            //std::cout << value << std::endl;
        }
    }
}

void LBM::externalForces() {
    /*for (int i = m_height/2 - 5; i < m_height/2 + 5; ++i) {
        m_densities(i, 1)[1] += 2.3;
    }*/

    /*for (int i = -50+m_height/2 - 5; i < -50+m_height/2 + 5; ++i) {
        m_densities(i, m_width-2)[3] += 2.3;
    }*/

}

void LBM::advection() {
    m_oldDensities = m_densities;
    m_densities.reset();

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            /*if (m_walls(i, j) != false)
                continue;*/
            /*m_densities(i, j)[0] = m_oldDensities(i, j)[0];
            if (m_walls(i, j+1) == false)
                m_densities(i, j+1)[1] = m_oldDensities(i, j)[1];
            else
                m_densities(i, j)[3] = m_oldDensities(i, j)[1];

            if (m_walls(i+1, j) == false)
                m_densities(i+1, j)[2] = m_oldDensities(i, j)[2];
            else
                m_densities(i, j)[4] = m_oldDensities(i, j)[2];

            if (m_walls(i, j-1) == false)
                m_densities(i, j-1)[3] = m_oldDensities(i, j)[3];
            else
                m_densities(i, j)[1] = m_oldDensities(i, j)[3];

            if (m_walls(i-1, j) == false)
                m_densities(i-1, j)[4] = m_oldDensities(i, j)[4];
            else
                m_densities(i, j)[2] = m_oldDensities(i, j)[4];

            if (m_walls(i+1, j+1) == false)
                m_densities(i+1, j+1)[5] = m_oldDensities(i, j)[5];
            else
                m_densities(i, j)[7] = m_oldDensities(i, j)[5];

            if (m_walls(i+1, j-1) == false)
                m_densities(i+1, j-1)[6] = m_oldDensities(i, j)[6];
            else
                m_densities(i, j)[8] = m_oldDensities(i, j)[6];

            if (m_walls(i-1, j-1) == false)
                m_densities(i-1, j-1)[7] = m_oldDensities(i, j)[7];
            else
                m_densities(i, j)[5] = m_oldDensities(i, j)[7];

            if (m_walls(i-1, j+1) == false)
                m_densities(i-1, j+1)[8] = m_oldDensities(i, j)[8];
            else
                m_densities(i, j)[6] = m_oldDensities(i, j)[8];*/
            int xplus  = ((j==m_width-1) ? (0) : (j+1));
            int xminus = ((j==0) ? (m_width-1) : (j-1));
            int yplus  = ((i==m_height-1) ? (0) : (i+1));
            int yminus = ((i==0) ? (m_height-1) : (i-1));
            m_densities(i, j)[0] = m_oldDensities(i, j)[0];
            m_densities(i, j)[1] = m_oldDensities(i, xminus)[1];
            m_densities(i, j)[2] = m_oldDensities(yminus, j)[2];
            m_densities(i, j)[3] = m_oldDensities(i, xplus)[3];
            m_densities(i, j)[4] = m_oldDensities(yplus, j)[4];
            m_densities(i, j)[5] = m_oldDensities(yminus, xminus)[5];
            m_densities(i, j)[6] = m_oldDensities(yminus, xplus)[6];
            m_densities(i, j)[7] = m_oldDensities(yplus, xplus)[7];
            m_densities(i, j)[8] = m_oldDensities(yplus, xminus)[8];

        }
    }

}
float LBM::collision(float dt) {
    // Rho
    //m_rho.reset();
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            float rho = (m_densities(i, j)[0] + m_densities(i, j)[1] + m_densities(i, j)[2] +
                         m_densities(i, j)[3] + m_densities(i, j)[4] + m_densities(i, j)[5] +
                         m_densities(i, j)[6] + m_densities(i, j)[7] + m_densities(i, j)[8]);
            if (m_walls(i, j) == true) {
                rho = 1.0f;
            }
            m_rho(i, j) = rho;
        }
    }

    // Velocity
    float maxV = 0.0;
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {

            m_velocities(i, j) =
                   (m_densities(i, j)[1]*Vec2{0.0, 1.0} + m_densities(i, j)[2]*Vec2{1.0, 0.0} + m_densities(i, j)[3]*Vec2{0.0, -1.0} +
                    m_densities(i, j)[4]*Vec2{-1.0, 0.0} + m_densities(i, j)[5]*Vec2{1.0, 1.0} + m_densities(i, j)[6]*Vec2{1.0, -1.0} +
                    m_densities(i, j)[7]*Vec2{-1.0, -1.0} + m_densities(i, j)[8]*Vec2{-1.0, 1.0}) / m_rho(i, j);
            if (norm(m_velocities(i, j)) > 0.2f)
                m_velocities(i, j) = m_velocities(i, j) * 0.2 / norm(m_velocities(i, j));
            if (j == 0 || j == m_width-1) {
                m_rho(i, j) = 1.0f;
                m_velocities(i, j) = Vec2{0.0, 0.1};
            }
            if (m_walls(i, j) == true) {
                m_velocities(i, j) = Vec2::zero();

            }

            if (norm(m_velocities(i, j)) > maxV)
                maxV = norm(m_velocities(i, j));
        }
    }

    // Update
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            Vec2 v = m_velocities(i, j);
            float lastTerm = - 1.5f*dot(v, v);
            for (int k = 0; k < 9; ++k) {

                float eiu = dot(m_e[k], v);
                float feq = m_rho(i, j) * m_weights[k] * (1.0f + 3.0f*eiu + 4.5f*eiu*eiu + lastTerm);
                m_densities(i, j)[k] = (1.0f - m_invTau) * m_densities(i, j)[k] + m_invTau * (feq);
                if (m_walls(i, j) == true || j == 0 || j == m_width-1) {
                    m_densities(i, j)[k] = feq;
                }
                //m_densities(i, j)[k] +=  m_invTau * (feq - m_densities(i, j)[k]);
            }
        }
    }
    return maxV;
}