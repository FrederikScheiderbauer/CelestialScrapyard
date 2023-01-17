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
#include "../headers/LightSource.hpp"

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
    glUniform1i(glGetUniformLocation(planetProgram->name, "crater"), 0);
    std::vector<Texture> planet_textures = {grassland_texture,water_texture,mountain_texture, snow_texture, crater_texture};

    glUniform1i(glGetUniformLocation(planetProgram->name, "grassland_cube"), 6);
    glUniform1i(glGetUniformLocation(planetProgram->name, "water_cube"), 7);
    glUniform1i(glGetUniformLocation(planetProgram->name, "mountain_cube"), 8);
    glUniform1i(glGetUniformLocation(planetProgram->name, "snow_cube"), 9);
    glUniform1i(glGetUniformLocation(planetProgram->name, "crater_cube"), 1);
    std::vector<Texture> planet_textures_cube = {grassland_texture_cube,water_texture_cube,mountain_texture_cube, snow_texture_cube, crater_texture_cube};


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,planet_textures[4].id);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[0].id);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[1].id);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[2].id);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[3].id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP,planet_textures_cube[4].id);
}

Planet::Planet(unsigned long noiseSeed) {
    planetProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    //generate planet Textures
    set_Textures();

    noise = new Noise(noiseSeed, Noise::continental);
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, *noise, PLANET_RESOLUTION);
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

    create_Forests(noiseSeed);
}

void Planet::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(planetProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Planet::handleCollisions() {
    for (auto &item : vertexUpdateQueue) {
        if (--item.collisionCounter <= 0) {
            particleQueue.push_back({item.craterCenter, 0});
        }
    }
}

void Planet::drawParticles(int width, int height) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    //compute random point on sphere around crater for each particle
    //https://math.stackexchange.com/questions/1585975/how-to-generate-random-points-on-a-sphere
    float particleHeight = noise->getParticleHeight();
    for (int j = 0; j < particleQueue.size(); ++j) {
        if (++particleQueue[j].particleCounter <= PARTICLE_DURATION) {
            for (int i = 0; i < 100; ++i) {
                float x = Random::getFromNormalDistribution();
                float y = Random::getFromNormalDistribution();
                float z = Random::getFromNormalDistribution();
                float r = Random::getInRange(0.8f, 1.3f);
                particle.Position = r * (particleHeight * particleQueue[j].craterCenter + 0.2f * glm::normalize(glm::vec3(x,y,z)));
                particle.CraterCenter = particleQueue[j].craterCenter;
                particleSystem.emit(particle);
            }
        } else {
            particleQueue.erase(particleQueue.begin() + j);
        }
    }
    particleSystem.draw(width, height);
    glDisable(GL_BLEND);
}

void Planet::checkForVertexUpdate() {
    //check if vertex data has been updated
    if (vertexUpdateInProgress && vertexUpdateQueue.front().collisionCounter <= 0) {
        std::chrono::milliseconds waitTime(0);
        if (currentVertexUpdate.wait_for(waitTime) == std::future_status::ready) {
            auto changed = currentVertexUpdate.get();
            for(int i = 0; i < CUBE_NUM_FACES; ++i) {
                if (changed[i]) {
                    cubefaces[i]->updateGPUBuffer();
                    //destroy trees only on changed cubefaces

                }
            }
            glm::vec3 collisionPoint = std::get<0>(cubefaces[0]->displacePointOnUnitSphere(vertexUpdateQueue.front().craterCenter));
            destroy_trees(collisionPoint,0.25f);
            vertexUpdateQueue.pop_front();
            vertexUpdateInProgress = false;
            dispatchVertexUpdate();
            //destroy_trees(vertexUpdateQueue.front().craterCenter,0.35f);
        }
    }
}


void Planet::draw(int width, int height, glm::vec3 &planet_info) {
    handleCollisions();
    drawParticles(width, height);
    checkForVertexUpdate();

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
    LightSource::getInstance().bindToShader(planetProgram->name);


    //
    glUniform3fv(glGetUniformLocation(planetProgram->name, "planet_info"), 1, &planet_info[0]);

    //

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }

    //draw trees
    pineTreeModel.draw_instanced(width,height,tree_transformation_matrices, planet_info);

}

void Planet::addCrater(glm::vec3 throwDirection, float throwSpeed) {
    glm::vec3 pointOnUnitSphere = glm::normalize(throwDirection);
    glm::vec3 collisionPoint = std::get<0>(cubefaces[0]->displacePointOnUnitSphere(pointOnUnitSphere));
    float tCollision = glm::abs(collisionPoint.x / throwDirection.x);
    int collisionCounter = (tCollision - 1) / throwSpeed;
    vertexUpdateQueue.push_back({pointOnUnitSphere, collisionCounter});
    dispatchVertexUpdate();
    //destroy_trees(collisionPoint,0.35f);
}

void Planet::dispatchVertexUpdate() {
    if (vertexUpdateInProgress || vertexUpdateQueue.empty())
        return;
    glm::vec3 center = vertexUpdateQueue.front().craterCenter;
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
//make sure there arent too many trees per side
//make sure there each position vertex only appears once
//make sure trees arent too close to each other
std::vector<glm::vec3> sanity_check(std::vector<glm::vec3>& vector) {
    std::vector<glm::vec3> results;
    results.push_back(vector[0]);
    results.push_back(vector[1]);

    for (int i = 2; i < vector.size(); i+=2) {
        glm::vec3 pos_vertex = vector[i];
        bool to_be_added = true;
        for (int j = 0; j < results.size();j+=2) {
            if(glm::abs(glm::length(pos_vertex-results[j])) < 0.0005f) {
                to_be_added = false;
                break;
            }
        }
        if(to_be_added) {
//            float rand_x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.01f));
//            float rand_y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.01f));
//            float rand_z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.01f));
//            results.push_back(vector[i]+glm::vec3(rand_x,rand_y,rand_z));
//            results.push_back(vector[i+1]+glm::vec3(rand_x,rand_y,rand_z));
            results.push_back(vector[i]);
            results.push_back(vector[i+1]);
       }
    }
    if(vector.size() > 10000) {
        vector.resize(10000);
    }
    return results;
}
void Planet::create_Forests(unsigned long noiseSeed){
    Noise noise = Noise(noiseSeed, Noise::forests);
    for ( int i = 0; i < CUBE_NUM_FACES; i++) {
        std::vector<glm::vec3> offsets = cubefaces[i]->filter_vertices_and_normals_from_map(noise);
        std::vector<glm::vec3> tree_positions = sanity_check(offsets);
            for ( int j = 0; j < tree_positions.size(); j++) {
                treeOffsets.push_back(tree_positions[j]);
            }
    }
    calculate_tree_transformations(treeOffsets);
}
void Planet::calculate_tree_transformations(std::vector<glm::vec3>& offsets){
    //offsets2 consist of vec3s which apepar in pairs: 0:pos_vector;1:normal_vector...n-2:pos_vector;n-1:normal_vector
    std::vector<glm::mat4> transformation_matrices;
    float tree_scaling = 0.001f;
    for(int i = 0; i< offsets.size();i +=2) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 model_pos_offset = offsets[i];
        glm::vec3 model_normal_offset = offsets[i+1];

        model = glm::translate(model,model_pos_offset);

        model = glm::scale(model,glm::vec3(tree_scaling));

        glm::vec3 localUp = glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 origin = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 direction = glm::normalize(model_pos_offset);

        glm::vec3 rotAxis = glm::normalize(glm::cross(localUp,(model_normal_offset)));
        float rotAngle = glm::acos(glm::dot(localUp,model_normal_offset));

        model = glm::rotate(model,rotAngle,rotAxis);
        transformation_matrices.push_back(model);
    }
    tree_transformation_matrices = transformation_matrices;
}

void Planet::destroy_trees(glm::vec3 crater_center,float radius) {
    std::vector<glm::vec3> new_trees;
    for (int i = 0; i < treeOffsets.size(); i+=2) {
        if(glm::length(treeOffsets[i]-crater_center) > radius) {
            new_trees.push_back(treeOffsets[i]);
            new_trees.push_back(treeOffsets[i+1]);
        }
    }
    treeOffsets = new_trees;
    calculate_tree_transformations(treeOffsets);
}

void Planet::plant_trees(glm::vec3 forest_center,float radius) {
    glm::vec3 pointOnUnitSphere = glm::normalize(forest_center);
    glm::vec3 collisionPoint = std::get<0>(cubefaces[0]->displacePointOnUnitSphere(pointOnUnitSphere));
    std::vector<glm::vec3> new_trees;
    for (int i=0; i < cubefaces.size(); i++) {
        std::vector<glm::vec3> tree_vertices = cubefaces[i]->get_Surface_Vertices_On_Surface(collisionPoint,radius);
        if(tree_vertices.empty()) {
            continue;
        }
        std::vector<glm::vec3> actual_trees = sanity_check(tree_vertices);
        for( int j = 0; j < actual_trees.size(); j++) {
            treeOffsets.push_back(actual_trees[j]);
        }
    }

    calculate_tree_transformations(treeOffsets);
}

glm::vec3 Planet::getPlanetInfo() {
    return noise->getPlanetInfo();
}
