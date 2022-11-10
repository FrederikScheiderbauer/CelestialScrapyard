#ifndef SPHERE_H
#define SPHERE_H

#include "ShaderProgram/ShaderProgram.hpp"
#include "CubeFace.hpp"
#include <glad/glad.h>
#include <memory>
#include <array>
#include <glm/glm.hpp>

#define CUBE_NUM_FACES 6

class Sphere
{
private:
    std::unique_ptr<ShaderProgram> sphereProgram;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
public:
    Sphere();
    void draw();
};

#endif
