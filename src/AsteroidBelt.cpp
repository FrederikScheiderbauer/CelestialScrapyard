#include "../headers/AsteroidBelt.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../headers/camera.hpp"
#include "config/config.h"
#include "../headers/Random.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/asteroidBelt.vert", (std::string)Project_SOURCE_DIR + "/src/shader/asteroidBelt.frag"};
const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

AsteroidBelt::AsteroidBelt(unsigned long noiseSeed) {
    asteroidBeltProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    Noise noise = Noise(noiseSeed, Noise::asteroid);
    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        glm::vec3 direction = directions[i];
        cubefaces[i] = std::make_unique<CubeFace>(direction, noise, ASTEROID_RESOLUTION);
    }

    for(int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->addEdgeNormals(cubefaces);
        cubefaces[i]->uploadToGPU();
    }

    offsets = new glm::vec4[NUM_ASTEROIDS];
    for (int i = 0; i < NUM_ASTEROIDS; ++i) {
        float x_pos = Random::getInRange(-1.f, 1.f);
        float y_pos = Random::getInRange(-0.05f, 0.05f);
        float z_pos = Random::getInRange(-1.f, 1.f);
        offsets[i] = 4.f * glm::normalize(glm::vec4(x_pos, y_pos, z_pos, 0.f));
    }

    glGenBuffers(1, &offsetBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * NUM_ASTEROIDS, offsets, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void AsteroidBelt::setUniformMatrix(glm::mat4 matrix, std::string type) {
    glUniformMatrix4fv(glGetUniformLocation(asteroidBeltProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void AsteroidBelt::draw(int width, int height) {
    Camera* camera = Camera::get_Active_Camera();

    asteroidBeltProgram->use();
    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(0.1f);
    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    setUniformMatrix(model,"model");

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = camera->get_View_Matrix();
    //Camera* camera2 = Camera::get_Active_Camera();
    //view = camera2->get_View_Matrix();
    setUniformMatrix(view,"view");

    glUniform3fv(glGetUniformLocation(asteroidBeltProgram->name, "cameraPos"), 1, &cameraPos[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsetBuffer);

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->drawInstanced(NUM_ASTEROIDS);
    }
}

AsteroidBelt::~AsteroidBelt() {
    delete[] offsets;
}
