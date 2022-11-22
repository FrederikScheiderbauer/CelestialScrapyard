#include "Sphere.hpp"
#include "definitions.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "config/config.h"
#include "PerlinNoise/PerlinNoise.hpp"

std::string source_Directory = Project_SOURCE_DIR;

const std::vector<std::string> SHADER_PATHS = {source_Directory +"/src/shader/sphere.vert", source_Directory + "/src/shader/sphere.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

Sphere::Sphere() {
    sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    const siv::PerlinNoise::seed_type seed = 123456u;
    siv::PerlinNoise perlin{seed};

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, perlin);
    }
}

void Sphere::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Sphere::draw(int width, int height) {
    Camera* camera = Camera::get_Active_Camera();

    sphereProgram->use();
    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    setUniformMatrix(model,"model");

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = glm::lookAt(cameraPos,camera->get_Camera_Target(),camera->get_Camera_Up());
    //Camera* camera2 = Camera::get_Active_Camera();
    //view = camera2->get_View_Matrix();
    setUniformMatrix(view,"view");

    glUniform3fv(glGetUniformLocation(sphereProgram->name, "cameraPos"), 1, &cameraPos[0]);

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}
