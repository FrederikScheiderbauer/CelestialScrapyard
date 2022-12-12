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
#include "texture.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/sphere.vert", (std::string)Project_SOURCE_DIR + "/src/shader/sphere.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

struct Planet_Info{
    float water_Level;
    float mountain_Height;
    float snow_peak_Height;
};


Sphere::Sphere(unsigned long noiseSeed) {
    sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    //generate planet Textures
        //generate grassland texture
        std::string grassland_path = (std::string)Project_SOURCE_DIR +"/src/assets/Grass 1.png";
        Texture grassland_texture = TextureLoader::generate_diffuse_texture(grassland_path);

        std::string mountain_path = (std::string)Project_SOURCE_DIR +"/src/assets/mountain texture.png";
        Texture mountain_texture = TextureLoader::generate_diffuse_texture(mountain_path);

        std::string snow_path = (std::string)Project_SOURCE_DIR +"/src/assets/snow1.jpg";
        Texture snow_texture = TextureLoader::generate_diffuse_texture(snow_path);

        std::string water_path = (std::string)Project_SOURCE_DIR +"/src/assets/00water-texture.png";
        Texture water_texture = TextureLoader::generate_diffuse_texture(water_path);

    sphereProgram->use(); // don't forget to activate the shader before setting uniforms! 
    glUniform1i(glGetUniformLocation(sphereProgram->name, "grassland"), 1);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "water"), 2);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "mountain"), 3);
    glUniform1i(glGetUniformLocation(sphereProgram->name, "snow"), 4);
    std::vector<Texture> planet_textures = {grassland_texture,water_texture,mountain_texture, snow_texture};

    Noise noise = Noise(noiseSeed, Noise::mountainous);
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, noise, planet_textures);
    }

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->addEdgeNormals(cubefaces);
        cubefaces[i]->uploadToGPU();
    }

    particle.ColorBegin = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    particle.ColorEnd = { 98 / 255.0f, 87 / 255.0f, 60 / 255.0f, 1.0f };
    particle.SizeBegin = 0.02f, particle.SizeVariation = 0.3f, particle.SizeEnd = 0.0f;
    particle.LifeTime = 100;
    particle.Velocity = { 0.0f, 0.0f, 0.0f};
    //particle.VelocityVariation = { 3.0f, 1.0f };
    particle.Position = { 0.0f, 0.0f, 0.0f };
}

void Sphere::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Sphere::drawParticles(int width, int height) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    for (int i = 0; i < 100; ++i) {
        //https://stackoverflow.com/questions/38244877/how-to-use-stdnormal-distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> d;
        float x = d(gen);
        float y = d(gen);
        float z = d(gen);
        //https://stackoverflow.com/questions/686353/random-float-number-generation
        float r = 0.9 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.1-0.9)));
        //point on sphere: https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability
        particle.Position = r * (2.f * currentCraterCenter + 0.2f * glm::normalize(glm::vec3(x,y,z)));
        particle.CraterNormal = glm::normalize(currentCraterCenter);

        particleSystem.emit(particle);
    }
    particleSystem.draw(width, height);
    glDisable(GL_BLEND);
}

void Sphere::draw(int width, int height, glm::vec3 &planet_info) {
    //check if vertex data has been updated
    if (vertexUpdateInProgress) {
        drawParticles(width, height);

        std::chrono::milliseconds waitTime(0);
        if (vertexUpdateFuture.wait_for(waitTime) == std::future_status::ready) {
            auto changed = vertexUpdateFuture.get();
            for(int i = 0; i < CUBE_NUM_FACES; ++i) {
                if (changed[i]) {
                    cubefaces[i]->updateGPUBuffer();
                }
            }
            vertexUpdateInProgress = false;
            particleSystem.setAllInactive();
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


    //
    glUniform3fv(glGetUniformLocation(sphereProgram->name, "planet_info"), 1,&planet_info[0]);

    //

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}

void Sphere::addCrater(glm::vec3 center) {
    vertexUpdateFuture = std::async(std::launch::async, [this, center](){
        return this->recomputeVertexDataAsync(center);
    });
    vertexUpdateInProgress = true;
    currentCraterCenter = center;
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
