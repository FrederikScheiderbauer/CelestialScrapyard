#ifndef SPHERE_H
#define SPHERE_H

#include "ShaderProgram.hpp"
#include "CubeFace.hpp"
#include <glad/glad.h>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <future>
#include <queue>
#include "camera.hpp"
#include "ParticleSystem.hpp"
#include "Tree.hpp"


struct Planet_Config {
    float water_level;
    float mountain_range_start;
    float snow_peak_start;
};

class Planet
{
private:
    //PineTree pineTreeModel;
    std::unique_ptr<ShaderProgram> planetProgram;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    Noise *noise;
    const int PLANET_RESOLUTION = 200;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
    void setUniformMatrix(glm::mat4 matrix, std::string type);
    GLuint textureID;

    struct VertexQueueItem {
        glm::vec3 craterCenter;
        float asteroidSize;
        int collisionCounter;
    };
    std::deque<VertexQueueItem> vertexUpdateQueue;
    struct ParticleQueueItem {
        glm::vec3 craterCenter;
        float asteroidSize;
        int particleCounter;
    };
    std::vector<ParticleQueueItem> particleQueue;
    std::future<std::array<bool, CUBE_NUM_FACES>> currentVertexUpdate;
    bool vertexUpdateInProgress = false;
    const int PARTICLE_DURATION = 80;
    void handleCollisions();
    void updateParticles();
    void checkForVertexUpdate();
    void dispatchVertexUpdate();

    std::vector<glm::vec3> treeOffsets;
    //PineTree pineTreeModel;
    ParticleProps particle;
    ParticleSystem particleSystem;
    PineTree pineTreeModel;
    void set_Textures();
    void calculate_tree_transformations(std::vector<glm::vec3>& offsets);
    void destroy_trees(glm::vec3 crater_center,float radius);
    
    std::vector<glm::mat4> tree_transformation_matrices;
    float random_value_list[100];
    std::vector<glm::vec3> sanity_check(std::vector<glm::vec3>& vector);
public:
    Planet(unsigned long noiseSeed);
    void drawParticles(int width, int height);
    void draw(int width, int height, glm::vec3 &planet_info);
    void drawForDepthMap(glm::vec3 &planet_info);
    void addCrater(glm::vec3 throwDirection, float asteroidSpeed, float asteroidSize);
    std::array<bool, CUBE_NUM_FACES> recomputeVertexDataAsync(glm::vec3 center, float asteroidSize);
    void create_Forests(unsigned long noiseSeed);
    void plant_trees(glm::vec3 forest_center,float radius);
    glm::vec3 getPlanetInfo();
};



class Ocean{

    private:
        std::unique_ptr<ShaderProgram> oceanProgram;
        std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
        const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
        const int compression_factor = 10;
        GLuint water_surface_texture;

    public:
        Ocean(unsigned int planet_resolution);
        void draw(int width, int height, glm::vec3 &planet_info);
};
#endif
