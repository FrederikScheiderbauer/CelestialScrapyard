#include "../headers/Planet.hpp"
#include "../headers/definitions.hpp"
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
#include "../headers/texture_loader.hpp"
#include "../headers/texture.hpp"
#include "../headers/Random.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/planet.vert", (std::string)Project_SOURCE_DIR + "/src/shader/planet.frag"};
const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

struct Planet_Info{
    float water_Level;
    float mountain_Height;
    float snow_peak_Height;
};

void Planet::set_Textures(){
//generate planet Textures
    std::string grassland_path = (std::string)Project_SOURCE_DIR +"/src/assets/Grass 1.png";
    std::string crater_path = (std::string)Project_SOURCE_DIR +"/src/assets/burnt_sand.png";
    std::string water_path = (std::string)Project_SOURCE_DIR +"/src/assets/00water-texture.png";
    std::string snow_path = (std::string)Project_SOURCE_DIR +"/src/assets/snow1.jpg";
    std::string mountain_path = (std::string)Project_SOURCE_DIR +"/src/assets/mountain texture.png";
    
    Texture grassland_texture = TextureLoader::generate_diffuse_texture(grassland_path);
    Texture mountain_texture = TextureLoader::generate_diffuse_texture(mountain_path);
    Texture snow_texture = TextureLoader::generate_diffuse_texture(snow_path);
    Texture water_texture = TextureLoader::generate_diffuse_texture(water_path);
    Texture crater_texture = TextureLoader::generate_diffuse_texture(crater_path);

    Texture grassland_texture_cube = TextureLoader::generate_One_Texture_As_cubemap(grassland_path);
    Texture mountain_texture_cube = TextureLoader::generate_One_Texture_As_cubemap(mountain_path);
    Texture snow_texture_cube = TextureLoader::generate_One_Texture_As_cubemap(snow_path);
    Texture water_texture_cube = TextureLoader::generate_One_Texture_As_cubemap(water_path);
    Texture crater_texture_cube = TextureLoader::generate_One_Texture_As_cubemap(crater_path);

    planetProgram->use(); // don't forget to activate the shader before setting uniforms!
    glUniform1i(glGetUniformLocation(planetProgram->name, "crater"), 5);
    std::vector<Texture> planet_textures = {grassland_texture,water_texture,mountain_texture, snow_texture, crater_texture};

    glUniform1i(glGetUniformLocation(planetProgram->name, "grassland_cube"), 6);
    glUniform1i(glGetUniformLocation(planetProgram->name, "water_cube"), 7);
    glUniform1i(glGetUniformLocation(planetProgram->name, "mountain_cube"), 8);
    glUniform1i(glGetUniformLocation(planetProgram->name, "snow_cube"), 9);
    glUniform1i(glGetUniformLocation(planetProgram->name, "crater_cube"), 10);
    std::vector<Texture> planet_textures_cube = {grassland_texture_cube,water_texture_cube,mountain_texture_cube, snow_texture_cube, crater_texture_cube};


    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D,planet_textures[4].id);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[0].id);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[1].id);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[2].id);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[3].id);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[4].id);
}

Planet::Planet(unsigned long noiseSeed) {
    planetProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    //generate planet Textures
    set_Textures();

    Noise noise = Noise(noiseSeed, Noise::mountainous);
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, noise, PLANET_RESOLUTION);
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

    create_Forests();
}

void Planet::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(planetProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Planet::drawParticles(int width, int height) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    //compute random point on sphere around crater for each particle
    //https://math.stackexchange.com/questions/1585975/how-to-generate-random-points-on-a-sphere
    for (auto &craterCenter : vertexUpdateQueue) {
        for (int i = 0; i < 100; ++i) {
            float x = Random::getFromNormalDistribution();
            float y = Random::getFromNormalDistribution();
            float z = Random::getFromNormalDistribution();
            float r = Random::getInRange(0.8f, 1.3f);
            particle.Position = r * (2.f * craterCenter + 0.2f * glm::normalize(glm::vec3(x,y,z)));
            particle.CraterCenter = craterCenter;
            particleSystem.emit(particle);
        }
    }
    particleSystem.draw(width, height);
    glDisable(GL_BLEND);
}

void Planet::draw(int width, int height, glm::vec3 &planet_info) {
    drawParticles(width, height);
    //check if vertex data has been updated
    if (vertexUpdateInProgress) {
        std::chrono::milliseconds waitTime(0);
        if (currentVertexUpdate.wait_for(waitTime) == std::future_status::ready) {
            auto changed = currentVertexUpdate.get();
            for(int i = 0; i < CUBE_NUM_FACES; ++i) {
                if (changed[i]) {
                    cubefaces[i]->updateGPUBuffer();
                }
            }
            vertexUpdateQueue.pop_front();
            vertexUpdateInProgress = false;
            dispatchVertexUpdate();
        }
    }

    Camera* camera = Camera::get_Active_Camera();

    planetProgram->use();
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

    glUniform3fv(glGetUniformLocation(planetProgram->name, "cameraPos"), 1, &cameraPos[0]);


    //
    glUniform3fv(glGetUniformLocation(planetProgram->name, "planet_info"), 1, &planet_info[0]);

    //

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }

    //draw trees
    pineTreeModel.draw_instanced(width,height,treeOffsets);
    
}

void Planet::addCrater(glm::vec3 center) {
    vertexUpdateQueue.push_back(center);
    dispatchVertexUpdate();
}

void Planet::dispatchVertexUpdate() {
    if (vertexUpdateInProgress || vertexUpdateQueue.empty())
        return;
    glm::vec3 center = vertexUpdateQueue.front();
    currentVertexUpdate = std::async(std::launch::async, [this, center](){
        return this->recomputeVertexDataAsync(center);
    });
    vertexUpdateInProgress = true;
}

std::array<bool, CUBE_NUM_FACES> Planet::recomputeVertexDataAsync(glm::vec3 center) {
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


void Planet::create_Forests(){
    
    std::vector<glm::vec3> treeOffsets_2;
    for ( int i = 0; i < CUBE_NUM_FACES; i++) {
        std::vector<glm::vec3> offsets = cubefaces[i]->filter_vertices_from_map();
        if(offsets.size() >10) {
            for ( int j = 0; j < 10; j++) {
                treeOffsets.push_back(offsets[j]);
            }
        }
    }
    treeOffsets.push_back(glm::vec3(2.0f,2.0f,2.0f));
    /*
    int index = 0;
    float offset = 0.1f;
    for(int y = -10; y < 10; y += 2)a
    {
        for(int x = -10; x < 10; x += 2)
        {
            glm::vec3 translation;
            translation.x = (float)x  + offset;
            translation.y = (float)y  + offset;
            translation.z = 0.0f;
            treeOffsets.push_back(translation);
        }
    } 
    */
    
}
