#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#define CUBE_NUM_FACES 6

class CubeFace
{
private:
    GLuint VAO;
    const int resolution = 10;
    glm::vec3 localUp;
    glm::vec3 axisA;
    glm::vec3 axisB;

public:
    CubeFace(glm::vec3 localUp);
    void draw();
};
#endif



