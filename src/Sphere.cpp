#include "Sphere.hpp"
#include <string>
#include <vector>
#include <filesystem>

const std::vector<std::string> SHADER_PATHS = {"../src/shader/sphere.vert", "../src/shader/sphere.frag"};
const std::vector<std::string> SHADER_PATHS_WINDOWS = {"../../src/shader/sphere.vert", "../../src/shader/sphere.frag"}; //TODO automate

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

Sphere::Sphere() {
    if(std::filesystem::exists("../../src/shader/")) {
        sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS_WINDOWS, SHADER_TYPES);
    } else {
        sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    }

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction);
    }
}

void Sphere::draw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sphereProgram->use();
    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}
