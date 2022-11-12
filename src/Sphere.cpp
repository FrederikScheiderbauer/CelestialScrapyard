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

void Sphere::setProjectionMatrix(glm::mat4 projection){
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
void Sphere::setModelMatrix(glm::mat4 model){
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, "model"), 1, GL_FALSE, glm::value_ptr(model));
}
void Sphere::setViewMatrix(glm::mat4 view){
    glUniformMatrix4fv(glGetUniformLocation(sphereProgram->name, "view"), 1, GL_FALSE, glm::value_ptr(view));
}

void Sphere::draw(Camera* camera) {

    sphereProgram->use();
    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)512/(float)512, 0.1f, 100.0f);
    setProjectionMatrix(proj);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    setModelMatrix(model);

    glm::mat4 view = glm::mat4(1.0f);
    //view = glm::lookAt(glm::vec3(camX/3.0, 0.0, camZ/3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = glm::lookAt(cameraPos,camera->get_Camera_Target(),camera->get_Camera_Up());
    setViewMatrix(view);
    //

    glUniform3fv(glGetUniformLocation(sphereProgram->name, "cameraPos"), 1, &cameraPos[0]);

    for (int i = 0; i < CUBE_NUM_FACES; ++i) {
        cubefaces[i]->draw();
    }
}
