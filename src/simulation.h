#ifndef EFS_SIMULATION_H
#define EFS_SIMULATION_H

#include "vec2.h"
#include "grid.h"
#include "staggeredGrid.h"

class Simulation {
public:
    Simulation(int w, int h);
    void step(float deltaTime, unsigned char* buffer);

private:
    void addExternalForces(float deltaTime);
    void projectIncompressible(float deltaTime);
    void advection(float deltaTime);
    float sampleField(const Vec2& v, std::vector<float>& field);

    int m_width;
    int m_height;

    mutable int cpt = 0;

    // Grid information
    StaggeredGrid<float> m_velocity;
    StaggeredGrid<float> m_oldVelocity;
    Grid<uint8_t> m_walls;
    Grid<float> m_pressure;

    // Parameters
    float m_g;
    float m_overRelaxation;
    int m_maxIter;
    float m_density;
    float m_spacing;
};


#endif //EFS_SIMULATION_H


