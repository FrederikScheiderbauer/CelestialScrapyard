#ifndef SPHERE_H
#define SPHERE_H

#include "ShaderProgram/ShaderProgram.hpp"
#include <glad/glad.h>
#include <memory>

class Sphere
{
private:
    GLuint VAO;
    std::unique_ptr<ShaderProgram> sphereProgram;
public:
    Sphere();
    void draw();
};

#endif
