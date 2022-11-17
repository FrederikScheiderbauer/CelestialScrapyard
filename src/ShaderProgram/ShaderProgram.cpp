#include "ShaderProgram.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <memory>

class Shader
{
private:

    //https://stackoverflow.com/questions/32842617/read-glsl-shaders-from-file
    std::string readShaderFile(std::string &pathToFile) {
        std::string content;
        std::ifstream fileStream(pathToFile.c_str(), std::ios::in);

        if(!fileStream.is_open()) {
            std::cerr << "Could not read file " << pathToFile << ". File does not exist." << std::endl;
            return "";
        }

        std::string line = "";
        while(!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }

    //https://stackoverflow.com/questions/35378656/opengl-not-reporting-shader-compiler-errors
    void checkForCompileErrors() {
        GLint status;
        glGetShaderiv(this->shaderName, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infolog_length;
            glGetShaderiv(this->shaderName, GL_INFO_LOG_LENGTH, &infolog_length);

            char *infolog = new char[infolog_length+1];
            glGetShaderInfoLog(this->shaderName, infolog_length, NULL, infolog);

            std::cout << this->shaderName << ": " << infolog << std::endl;
            delete[] infolog;
        }
    }

public:
    GLuint shaderName;
    Shader(std::string path, GLenum shaderType) {
        std::string shaderSource = readShaderFile(path);
        shaderName = glCreateShader(shaderType);
        const char* shaderSource_ptr = shaderSource.c_str();
        glShaderSource(shaderName, 1, &shaderSource_ptr, NULL);
        glCompileShader(shaderName);
        checkForCompileErrors();
    }

    GLuint getName() const {
        return this->shaderName;
    }

    ~ Shader() {
        glDeleteShader(shaderName);
    }
};

ShaderProgram::ShaderProgram(const std::vector<std::string> &shaderPaths, const std::vector<GLenum> &shaderTypes) {
    assert (shaderPaths.size() == shaderTypes.size());
    std::vector<std::shared_ptr<Shader>> shaders;
    for (int i = 0; i < shaderPaths.size(); ++i) {
        auto shader = std::make_shared<Shader>(shaderPaths[i], shaderTypes[i]);
        shaders.push_back(shader);
    }

    this->name = glCreateProgram();
    for(auto shader : shaders) {
        glAttachShader(name, shader->getName());
    }
    glLinkProgram(name);
    checkForLinkingErrors();
}

void ShaderProgram::use() {
    glUseProgram(name);
}

//based on: https://stackoverflow.com/questions/35378656/opengl-not-reporting-shader-compiler-errors
void ShaderProgram::checkForLinkingErrors() {
    GLint status;
    glGetProgramiv(this->name, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infolog_length;
        glGetProgramiv(this->name, GL_INFO_LOG_LENGTH, &infolog_length);

        char *infolog = new char[infolog_length+1];
        glGetProgramInfoLog(this->name, infolog_length, NULL, infolog);

        std::cout << this->name << ": " << infolog << std::endl;
        delete[] infolog;
    }
}
