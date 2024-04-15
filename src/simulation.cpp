#include "simulation.h"
#include <iostream>

Simulation::Simulation(int w, int h) :
    m_width(w), m_height(h), m_velocity(w, h, 0.0f), m_oldVelocity(w, h, 0.0f), m_walls(w, h, false),
    m_pressure(w, h, 0.0f),
    m_g(-9.81f), m_overRelaxation(1.9f), m_maxIter(100), m_density(1000.0), m_spacing(1.0f/float(h)) {

    for (int i = 0; i < h; ++i) {
        m_walls(i, 0) = true;
        m_walls(i, w-1) = true;
    }

    for (int j = 0; j < w; ++j) {
        m_walls(0, j) = true;
        m_walls(h-1, j) = true;
    }
}

void Simulation::step(float deltaTime, unsigned char* buffer) {
    /*std::cout << deltaTime << std::endl;

    for (int i = 0; i < m_width * m_height * 3; ++i) {
        buffer[i] = (i/m_width + (cpt%256)*i + i + cpt) % 256;
    }
    cpt++;*/

    // External forces

    //std::copy(std::begin(m_walls), std::end(m_walls), buffer);
    /*cpt++;
    cpt = cpt % (m_width * m_height * 3);
    for (int i = 0; i < m_width * m_height * 3; ++i) {
        buffer[i] = m_walls[i/3];
        /*buffer[i] = 0;
        if (i < cpt)
            buffer[i] = 255;*/
    //}

    if (deltaTime < 0.001)
        deltaTime = 0.001;

    addExternalForces(deltaTime);
    projectIncompressible(deltaTime);
    advection(deltaTime);

    for (int i = 0; i < m_width * m_height * 3; ++i) {
        buffer[i] = m_pressure[i/3];
        //std::cout << "avant : " << i << std::endl;
        //std::cout << "size : " << m_pressure.m_data.size() << std::endl;
        std::cout << m_pressure[i/3] << std::endl;
        //std::cout << "apres" << std::endl;
    }
}

void Simulation::addExternalForces(float deltaTime) {
    for (int i = 1; i < m_height ; ++i) {
        for (int j = 0; j < m_width; ++j) {
            if (m_walls(i, j) != true && m_walls(i-1, j) != true){}
                m_velocity.horizontal(i, j) += deltaTime * m_g;
        }
    }
}

void Simulation::projectIncompressible(float deltaTime) {

    float pressureFactor = m_density * m_spacing / deltaTime;

    for (int n = 0; n < m_maxIter; ++n) {
        for (int i = 1; i < m_height-1; ++i) {
            for (int j = 1; j <  m_width-1; ++j) {
                if (m_walls(i, j) == true)
                    continue;

                float down = m_walls(i-1, j);
                float up = m_walls(i+1, j);
                float left = m_walls(i, j-1);
                float right = m_walls(i, j+1);
                float nNeighbours = up + down + left + right;
                if (nNeighbours == 0)
                    continue;


                float a = m_velocity.horizontal(i+1, j);
                float b = m_velocity.horizontal(i, j);
                float c = m_velocity.vertical(i, j+1);
                float d = m_velocity.vertical(i, j);
                float div = m_velocity.horizontal(i+1, j) - m_velocity.horizontal(i, j) +
                            m_velocity.vertical(i, j+1) - m_velocity.vertical(i, j);
                std::cout << div << std::endl;
                float coefficient = m_overRelaxation * div / nNeighbours;
                m_velocity.horizontal(i, j) += down * coefficient;
                m_velocity.horizontal(i+1, j) -= up * coefficient;
                m_velocity.vertical(i, j) += left * coefficient;
                m_velocity.vertical(i, j+1) -= right * coefficient;
                m_pressure(i, j) += pressureFactor * coefficient;
            }
        }
    }
}

void Simulation::advection(float deltaTime) {

    float halfSpacing = m_spacing * 0.5f;

    for (int i = 1; i < m_height; ++i) {
        for (int j = 1; j < m_width; ++j) {

            if (m_walls(i, j) != true && m_walls(i, j-1) != true && j < m_width-1) {
                float vy = (m_oldVelocity.horizontal(i, j) + m_oldVelocity.horizontal(i, j-1) +
                            m_oldVelocity.horizontal(i+1, j-1) + m_oldVelocity.horizontal(i+1, j)) * 0.25f;
                Vec2 v = Vec2{i * m_spacing - deltaTime * m_oldVelocity.vertical(i, j), j * m_spacing + halfSpacing - deltaTime * vy};
                float vx = sampleField(v, m_oldVelocity.rawVertical());
                m_velocity.vertical(i, j) = vx;
            }

            if (m_walls(i, j) != true && m_walls(i-1, j) != true && i < m_height-1) {
                float vx = (m_oldVelocity.vertical(i, j) + m_oldVelocity.vertical(i-1, j) +
                            m_oldVelocity.vertical(i, j+1) + m_oldVelocity.vertical(i-1, j+1)) * 0.25f;
                Vec2 v = Vec2{i * m_spacing + halfSpacing - deltaTime * vx, j * m_spacing - deltaTime * m_oldVelocity.horizontal(i, j)};
                float vy = sampleField(v, m_oldVelocity.rawVertical());
                m_velocity.vertical(i, j) = vy;
            }
        }
    }
    std::copy(m_oldVelocity.rawHorizontal().begin(), m_oldVelocity.rawHorizontal().end(), m_velocity.rawHorizontal().begin());
    std::copy(m_oldVelocity.rawVertical().begin(), m_oldVelocity.rawVertical().end(), m_velocity.rawVertical().begin());
}

float Simulation::sampleField(const Vec2& v, std::vector<float>& field) {
    float halfSpacing = m_spacing * 0.5f;
    float inverseSpacing = 1.0f / m_spacing;
}