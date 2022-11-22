#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "definitions.hpp"
#include <vector>
#include "PerlinNoise/PerlinNoise.hpp"

#define RESOLUTION 30
#define NUM_VERTICES RESOLUTION * RESOLUTION
#define NUM_INDICES (RESOLUTION - 1) * (RESOLUTION - 1) * 6

class CubeFace
{
private:
    GLuint VAO;
    glm::vec3 localUp;
    glm::vec3 axisA;
    glm::vec3 axisB;

public:
    CubeFace(glm::vec3 localUp, siv::PerlinNoise &perlin);
    void draw();
};
#endif



