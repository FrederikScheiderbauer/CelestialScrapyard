#include "Sphere.hpp"
#include <string>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

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
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)512/(float)512, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); 

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sphereProgram->use();
    unsigned int projectionLoc = glGetUniformLocation(sphereProgram->name, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

    unsigned int modelLoc = glGetUniformLocation(sphereProgram->name, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
    unsigned int viewLoc = glGetUniformLocation(sphereProgram->name, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));



    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}
