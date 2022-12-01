#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "definitions.hpp"
#include <vector>
#include "PerlinNoise/PerlinNoise.hpp"

#define RESOLUTION 200
//vertex position stored interleaved with vertex normal -> two vec3
#define NUM_VERTICES RESOLUTION * RESOLUTION * 2
#define NUM_INDICES (RESOLUTION - 1) * (RESOLUTION - 1) * 6

class CubeFace
{
private:
    std::vector<GLuint> texture_IDs;
    GLuint texture_Array_ID;
    GLuint VAO;
    glm::vec3 localUp;
    glm::vec3 axisA;
    glm::vec3 axisB;

public:
    CubeFace(glm::vec3 localUp, siv::PerlinNoise &perlin,GLuint texture_Array_ID,std::vector<GLuint> _texture_IDs);
    void draw();
};
#endif



