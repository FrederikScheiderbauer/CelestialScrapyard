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
    glm::vec4 *offsets;
    const int NUM_ASTEROIDS = 30;
public:
    AsteroidBelt(unsigned long noiseSeed);
    void draw(int width, int height);
    ~AsteroidBelt();
};


#endif //GGD_ASTEROIDBELT_HPP
