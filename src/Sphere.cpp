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

        //generate Mountain texture
        /*
        std::string mountain_path = (std::string)Project_SOURCE_DIR +"/src/assets/stone1.jpg";
        GLuint mountain_texture_ID = TextureLoader::generate_texture(mountain_path);

        //generate Snow texture
        std::string snow_path = (std::string)Project_SOURCE_DIR +"/src/assets/snow1.jpg";
        GLuint snow_texture_ID = TextureLoader::generate_texture(snow_path);
*/
        //generate Water texture
        std::string water_path = (std::string)Project_SOURCE_DIR +"/src/assets/00water-texture.png";
        GLuint water_texture_ID = TextureLoader::generate_texture(water_path);

    std::vector<std::string> paths = {(std::string)Project_SOURCE_DIR +"/src/assets/Grass 1.png",(std::string)Project_SOURCE_DIR +"/src/assets/burnt_sand.png"};
    GLuint texture_arrays = TextureLoader::generate_Texture_Array(paths);

    sphereProgram->use(); // don't forget to activate the shader before setting uniforms! 
    glUniform1i(glGetUniformLocation(sphereProgram->name, "textureArray"), 0);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "grassland"), 1);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "water"), 2);
    std::vector<GLuint> texture_IDs = {grassland_texture_ID,water_texture_ID};

    //const siv::PerlinNoise::seed_type seed = 123456u;
    //take lowest 32 bits of seed
    const siv::PerlinNoise::seed_type seed = noiseSeed;
    siv::PerlinNoise perlin{seed};

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, perlin, texture_arrays, texture_IDs);
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
