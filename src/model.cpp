#include "../headers/model.hpp"
#include <GLFW/glfw3.h>
#include <config/config.h>
#include "../headers/camera.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/tree.vert", (std::string)Project_SOURCE_DIR + "/src/shader/tree.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

Model::Model() {
    
}

void Model::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(modelShader->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
