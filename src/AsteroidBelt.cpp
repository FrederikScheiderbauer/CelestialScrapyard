#include <iostream>
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
        glm::vec4 asteroidCenter;
        float minDistance;
        //check that asteroids aren't too close, TODO: might run indefinitely if there are too many asteroids
        do {
            minDistance = std::numeric_limits<float>::max();
            float x_pos = Random::getInRange(-1.f, 1.f);
            float z_pos = Random::getInRange(-1.f, 1.f);
            float r = Random::getInRange(3.5f, 4.5f);

            asteroidCenter = r * glm::normalize(glm::vec4(x_pos, 0.f, z_pos, 0.f));
            for (int j = 0; j < i; ++j) {
                float distance = glm::length(asteroidCenter - offsets[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }
        } while (minDistance < 4.f * ASTEROID_RADIUS);
        offsets[i] = asteroidCenter;
    }

    glGenBuffers(1, &offsetBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * NUM_ASTEROIDS, offsets, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void AsteroidBelt::setUniformMatrix(glm::mat4 matrix, std::string type) {
    glUniformMatrix4fv(glGetUniformLocation(asteroidBeltProgram->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

glm::vec3 asteroidCenterToSpherical(glm::vec4 center) {
    //https://mathworld.wolfram.com/SphericalCoordinates.html
    float r = glm::length(glm::vec3(center));
    float phi = glm::acos(center.z / r);
    float theta = glm::atan(center.y, center.x);
    return {r, 0.5f * glm::pi<float>() - phi, theta};
}

glm::vec4 sphericalToAsteroidCenter(glm::vec3 spherical) {
    //https://mathworld.wolfram.com/SphericalCoordinates.html
    float x = spherical.x * glm::cos(spherical.z) * glm::cos(spherical.y);
    float y = spherical.x * glm::sin(spherical.z) * glm::cos(spherical.y);
    float z = spherical.x * glm::sin(spherical.y);
    return {x, y, z, 0.f};
}

void AsteroidBelt::move() {
    for (int i = 0; i < NUM_ASTEROIDS; ++i) {
        if (i == pickedID && throwInProgress) {
            //keep asteroid at center of planet after throw
            if (glm::all(glm::epsilonEqual(offsets[i], glm::vec4(0.f), 1E-1f))) {
                throwInProgress = false;
                pickedID = -1;
            } else {
                t += THROW_SPEED;
                offsets[i] = glm::vec4{throwDirection * t, 0.f};
            }
        } else {
            auto spherical = asteroidCenterToSpherical(offsets[i]);
            float speed = ASTEROID_SPEED * (glm::abs(spherical.z) < 0.5f * glm::pi<float>() ? 1.f : -1.f);
            spherical.y += speed;
            spherical.y = std::fmod(spherical.y, glm::pi<float>());
            offsets[i] = sphericalToAsteroidCenter(spherical);
        }
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * NUM_ASTEROIDS, offsets);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


glm::vec3 AsteroidBelt::throwTowardsCenter() {
    if (pickedID == -1 || throwInProgress)
        return glm::vec3(0.0f);
    throwDirection = offsets[pickedID];
    throwInProgress = true;
    t = 1.f;
    return throwDirection;
}

void AsteroidBelt::setCollisionPoint(glm::vec3 collisionPoint) {

}

void AsteroidBelt::prepareDraw(int width, int height, bool outlining) {
    Camera* camera = Camera::get_Active_Camera();

    asteroidBeltProgram->use();
    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(ASTEROID_RADIUS + (outlining ? 0.01f : 0.0f));
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
    glUniform1i(glGetUniformLocation(asteroidBeltProgram->name, "picking"), picking);
    glUniform1i(glGetUniformLocation(asteroidBeltProgram->name, "pickedID"), pickedID);
}

void AsteroidBelt::executeDraw() {
    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->drawInstanced(NUM_ASTEROIDS);
    }
}

void AsteroidBelt::draw(int width, int height) {
    if (!picking) {
        move();
    }

    //outline rendering based on https://learnopengl.com/Advanced-OpenGL/Stencil-testing
    glEnable(GL_STENCIL_TEST);

    //standard draw
    prepareDraw(width, height, false);
    glUniform1i(glGetUniformLocation(asteroidBeltProgram->name, "outlining"), 0);
    executeDraw();

    //draw picked asteroid again in stencil buffer and in solid color for outline rendering
    if (pickedID == -1) {
        return;
    }

    prepareDraw(width, height, false);
    glUniform1i(glGetUniformLocation(asteroidBeltProgram->name, "outlining"), 1);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    executeDraw();

    prepareDraw(width, height, true);
    glUniform1i(glGetUniformLocation(asteroidBeltProgram->name, "outlining"), 2);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    executeDraw();
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glDisable(GL_STENCIL_TEST);
}

void AsteroidBelt::pick(int width, int height, glm::vec2 mousePosition) {
    picking = true;
    glClear(GL_COLOR_BUFFER_BIT);
    draw(width, height);

    glMemoryBarrier(GL_PIXEL_BUFFER_BARRIER_BIT);

    //http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
    glFlush();
    glFinish();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned char data[4];
    glReadPixels((GLint) mousePosition.x, height - (GLint) mousePosition.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
    std::cout << pickedID << std::endl;
    picking = false;
}

AsteroidBelt::~AsteroidBelt() {
    delete[] offsets;
}

float AsteroidBelt::getThrowSpeed() {
    return THROW_SPEED;
}
