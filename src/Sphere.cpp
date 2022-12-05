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
#include "texture_loader.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/sphere.vert", (std::string)Project_SOURCE_DIR + "/src/shader/sphere.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

Sphere::Sphere(unsigned long noiseSeed) {
    sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    //generate planet Textures
        //generate grassland texture
        std::string grassland_path = (std::string)Project_SOURCE_DIR +"/src/assets/Grass 1.png";
        GLuint grassland_texture_ID = TextureLoader::generate_texture(grassland_path);

        std::string mountain_path = (std::string)Project_SOURCE_DIR +"/src/assets/mountain texture.png";
        GLuint mountain_texture_ID = TextureLoader::generate_texture(mountain_path);

        std::string snow_path = (std::string)Project_SOURCE_DIR +"/src/assets/snow1.jpg";
        GLuint snow_texture_ID = TextureLoader::generate_texture(snow_path);

        std::string water_path = (std::string)Project_SOURCE_DIR +"/src/assets/00water-texture.png";
        GLuint water_texture_ID = TextureLoader::generate_texture(water_path);

    std::vector<std::string> paths = {(std::string)Project_SOURCE_DIR +"/src/assets/Grass 1.png",(std::string)Project_SOURCE_DIR +"/src/assets/burnt_sand.png"};
    GLuint texture_arrays = TextureLoader::generate_Texture_Array(paths);

    sphereProgram->use(); // don't forget to activate the shader before setting uniforms! 
    glUniform1i(glGetUniformLocation(sphereProgram->name, "textureArray"), 0);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "grassland"), 1);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "water"), 2);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "mountain"), 3);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "snow"), 4);
    std::vector<GLuint> texture_IDs = {grassland_texture_ID,water_texture_ID,mountain_texture_ID, snow_texture_ID};

    Noise noise = Noise(noiseSeed, Noise::mountainous);
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, noise, texture_arrays, texture_IDs);
    }

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->addEdgeNormals(cubefaces);
        cubefaces[i]->uploadToGPU();
    }
}

void Sphere::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Sphere::draw(int width, int height) {
    //check if vertex data has been updated
    if (vertexUpdateInProgress) {
        std::chrono::milliseconds waitTime(0);
        if (vertexUpdateFuture.wait_for(waitTime) == std::future_status::ready) {
            auto changed = vertexUpdateFuture.get();
            for(int i = 0; i < CUBE_NUM_FACES; ++i) {
                if (changed[i]) {
                    cubefaces[i]->updateGPUBuffer();
                }
            }
            vertexUpdateInProgress = false;
        }
    }

    Camera* camera = Camera::get_Active_Camera();

    sphereProgram->use();
    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f)); 
    setUniformMatrix(model,"model");

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = camera->get_View_Matrix();
    //Camera* camera2 = Camera::get_Active_Camera();
    //view = camera2->get_View_Matrix();
    setUniformMatrix(view,"view");

    glUniform3fv(glGetUniformLocation(sphereProgram->name, "cameraPos"), 1, &cameraPos[0]);

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}

void Sphere::addCrater(glm::vec3 center) {
    vertexUpdateFuture = std::async(std::launch::async, [this, center](){
        return this->recomputeVertexDataAsync(center);
    });
    vertexUpdateInProgress = true;
}

std::array<bool, CUBE_NUM_FACES> Sphere::recomputeVertexDataAsync(glm::vec3 center) {
    std::array<bool, CUBE_NUM_FACES> changed;
    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        changed[i] = cubefaces[i]->addCrater(center);
    }
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        if (changed[i]) {
            cubefaces[i]->addEdgeNormals(cubefaces);
        }
    }
    return changed;
}
