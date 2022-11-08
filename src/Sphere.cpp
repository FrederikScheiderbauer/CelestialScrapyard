#include "Sphere.hpp"
#include <string>
#include <vector>
#include <filesystem>

const std::vector<std::string> SHADER_PATHS = {"../src/shader/sphere.vert", "../src/shader/sphere.frag"};
const std::vector<std::string> SHADER_PATHS_WINDOWS = {"../../src/shader/sphere.vert", "../../src/shader/sphere.frag"}; //TODO automate

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

//https://learnopengl.com/Getting-started/Hello-Triangle
Sphere::Sphere() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if(std::filesystem::exists("../../src/shader/")) {
        sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS_WINDOWS, SHADER_TYPES);
    } else {
        sphereProgram = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    }
}

void Sphere::draw() {
    sphereProgram->use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
