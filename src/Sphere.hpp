#ifndef SPHERE_H
#define SPHERE_H

#include "ShaderProgram/ShaderProgram.hpp"
#include "CubeFace.hpp"
#include <glad/glad.h>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <future>
#include <queue>
#include "camera.hpp"
#include "ParticleSystem.hpp"


struct Planet_Config {
    float water_level;
    float mountain_range_start;
    float snow_peak_start;
};

class Sphere
{
private:
    std::unique_ptr<ShaderProgram> sphereProgram;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
    void setUniformMatrix(glm::mat4 matrix, std::string type);
    GLuint textureID;
    std::deque<glm::vec3> vertexUpdateQueue;
    std::future<std::array<bool, CUBE_NUM_FACES>> currentVertexUpdate;
    bool vertexUpdateInProgress = false;

    ParticleProps particle;
    ParticleSystem particleSystem;
    void drawParticles(int width, int height);
    void dispatchVertexUpdate();
public:
    Sphere(unsigned long noiseSeed);
    void draw(int width, int height, glm::vec3 &planet_info);
    void addCrater(glm::vec3 center);
    std::array<bool, CUBE_NUM_FACES> recomputeVertexDataAsync(glm::vec3 center);
};

#endif
