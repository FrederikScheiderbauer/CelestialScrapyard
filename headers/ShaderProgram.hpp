#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glad/glad.h>
#include <vector>
#include <string>

class ShaderProgram
{
private:
    void checkForLinkingErrors();
public:
    GLuint name;
    ShaderProgram(const std::vector<std::string> &shaderPaths, const std::vector<GLenum> &shaderTypes);
    void use();
};

#endif
