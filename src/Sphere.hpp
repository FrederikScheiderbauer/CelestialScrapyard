#ifndef SPHERE_H
#define SPHERE_H

#include "ShaderProgram/ShaderProgram.hpp"
#include "CubeFace.hpp"
#include <glad/glad.h>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "camera.hpp"

class Sphere
{
private:
    std::unique_ptr<ShaderProgram> sphereProgram;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
    void setUniformMatrix(glm::mat4 matrix, std::string type);
    GLuint textureID;

public:
    Sphere(unsigned long noiseSeed);
    void draw(int width, int height);
};

#endif
