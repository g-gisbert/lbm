#ifndef EFS_LBM_H
#define EFS_LBM_H

#include "grid.h"
#include "vec2.h"
#include <array>

class LBM {
public:
    LBM(int w, int h);

    void step(float deltaTime, unsigned char* buffer);

private:
    void externalForces();
    void advection();
    float collision(float ft);

    int m_width;
    int m_height;

    float m_invTau;

    Grid<std::array<float, 9>> m_oldDensities;
    Grid<std::array<float, 9>> m_densities;
    Grid<float> m_rho;
    Grid<Vec2> m_velocities;
    Grid<uint8_t> m_walls;
    std::array<float, 9> m_weights = {4.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0};
    std::array<Vec2, 9> m_e = {Vec2{0.0, 0.0}, Vec2{0.0, 1.0}, Vec2{1.0, 0.0}, Vec2{0.0, -1.0}, Vec2{-1.0, 0.0},
                               Vec2{1.0, 1.0}, Vec2{1.0, -1.0}, Vec2{-1.0, -1.0}, Vec2{-1.0, 1.0}};

};


#endif //EFS_LBM_H
