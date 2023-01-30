#ifndef GGD_ASTEROIDBELT_HPP
#define GGD_ASTEROIDBELT_HPP


#include <memory>
#include "ShaderProgram.hpp"
#include "CubeFace.hpp"

class AsteroidBelt {
private:
    std::unique_ptr<ShaderProgram> asteroidBeltProgram;
    std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> cubefaces;
    const int ASTEROID_RESOLUTION = 50;
    const std::array<glm::vec3, CUBE_NUM_FACES> directions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)};
    void setUniformMatrix(glm::mat4 matrix, std::string type);

    GLuint offsetBuffer;
    GLuint scaleFactorBuffer;
    glm::vec4 *offsets;
    glm::vec4 *scaleFactors;
    //std::vector<glm::vec4> scaleFactors;
    std::vector<float> scaleFactors2;
    const int NUM_ASTEROIDS = 150;
    const float ASTEROID_RADIUS = 0.05f;
    const float ASTEROID_SPEED = 0.001f;

    void move(float &pickedAsteroidTheta);
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
    void draw(int width, int height, float &pickedAsteroidTheta);
    void drawForDepthMap();
    void pick(int width, int height, glm::vec2 mousePosition, float &pickedAsteroidTheta);
    glm::vec3 throwTowardsCenter();
    float getThrowSpeed();
    ~AsteroidBelt();
};


#endif //GGD_ASTEROIDBELT_HPP
