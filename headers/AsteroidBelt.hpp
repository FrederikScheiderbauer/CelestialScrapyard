#ifndef GGD_ASTEROIDBELT_HPP
#define GGD_ASTEROIDBELT_HPP


#include <memory>
#include "ShaderProgram.hpp"
#include "CubeFace.hpp"

class AsteroidBelt {
private:
    std::unique_ptr<ShaderProgram> asteroidBeltProgram;
    std::unique_ptr<ShaderProgram> asteroidBeltProgramDepthMap;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    const int ASTEROID_RESOLUTION = 50;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
    void setUniformMatrix(glm::mat4 matrix, std::string type);

    GLuint offsetBuffer;
    glm::vec4 *offsets;
    const int NUM_ASTEROIDS = 150;
    const float ASTEROID_RADIUS = 0.05f;
    const float ASTEROID_SPEED = 0.001f;

    void move();
    void prepareDraw(int width, int height, bool outlining);
    void executeDraw();

    bool picking = false;
    int pickedID = -1;
    struct ThrowInfo {
        glm::vec3 direction;
        float t;
        int instanceId;
    };
    std::vector<ThrowInfo> throwInfos;
    const float THROW_SPEED = -0.008f;
public:
    AsteroidBelt(unsigned long noiseSeed);
    void draw(int width, int height);
    void drawForDepthMap();
    void pick(int width, int height, glm::vec2 mousePosition);
    glm::vec3 throwTowardsCenter();
    float getThrowSpeed();
    ~AsteroidBelt();
};


#endif //GGD_ASTEROIDBELT_HPP
