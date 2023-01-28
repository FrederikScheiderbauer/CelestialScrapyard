#include <iostream>
#include "../headers/GBuffer.hpp"
#include "config/config.h"
#include "glm/vec3.hpp"
#include "../headers/camera.hpp"
#include "../headers/LightSource.hpp"
#include "../headers/Random.hpp"
#include "glm/gtx/string_cast.hpp"

const std::vector<std::string> LIGHTING_PASS_SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/quad.vert", (std::string)Project_SOURCE_DIR + "/src/shader/lightingPass.frag"};
const std::vector<std::string> SSAO_SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/quad.vert", (std::string)Project_SOURCE_DIR + "/src/shader/ssao.frag"};
const std::vector<std::string> BLOOM_SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/quad.vert", (std::string)Project_SOURCE_DIR + "/src/shader/bloom.frag"};
const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

GBuffer::GBuffer(int width, int height) {
    lightingPassShaderProgram = std::make_unique<ShaderProgram>(LIGHTING_PASS_SHADER_PATHS, SHADER_TYPES);
    ssaoShaderProgram = std::make_unique<ShaderProgram>(SSAO_SHADER_PATHS, SHADER_TYPES);
    bloomShaderProgram = std::make_unique<ShaderProgram>(BLOOM_SHADER_PATHS, SHADER_TYPES);
    quad = std::make_unique<Quad>();

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glGenTextures(1, &gPosition);
    glGenTextures(1, &gNormal);
    glGenTextures(1, &gAlbedoSpec);
    glGenTextures(1, &depthAndStencil);

    glGenTextures(1, &lightingPassTexture);
    glGenTextures(1, &bloomBuffer);

    glGenFramebuffers(1, &ssaoBuffer);
    glGenTextures(1, &ssaoTexture);

    glGenFramebuffers(1, &refractionBuffer);
    glGenTextures(1, &refractionTexture);

    glGenFramebuffers(1, &reflectionBuffer);
    glGenTextures(1, &reflectionTexture);

    allocateTextures(width, height);
    bindToFBO();
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "incomplete G-Buffer" << std::endl;
    }

    for (int i = 0; i < NUM_SSAO_SAMPLES; ++i) {
        float x = Random::getInRange(-1.f, 1.f);
        float y = Random::getInRange(-1.f, 1.f);
        float z = Random::getInRange(0.f, 1.f);
        float r = Random::getInRange(0.f, 1.f);
        ssaoKernel[i] = r * glm::normalize(glm::vec3(x, y, z));
    }

}

void GBuffer::allocateTextures(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, depthAndStencil);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glBindTexture(GL_TEXTURE_2D, lightingPassTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    allocatedWidth = width;
    allocatedHeight = height;
}

void GBuffer::bindToFBO() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAndStencil, 0);
}

void GBuffer::prepareGeometryPass(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    if (width != allocatedWidth || height != allocatedHeight) {
        allocateTextures(width, height);
    }

    bindToFBO();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GBuffer::finishGemoetryPass() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::blitDepthAndStencilBuffer() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    glBlitFramebuffer(0, 0, allocatedWidth, allocatedHeight, 0, 0, allocatedWidth, allocatedHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBlitFramebuffer(0, 0, allocatedWidth, allocatedHeight, 0, 0, allocatedWidth, allocatedHeight, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::executeSSAOPass(int width, int height, glm::vec3 &radiusBiasPower) {
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    ssaoShaderProgram->use();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(ssaoShaderProgram->name, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glm::mat4 view = Camera::get_Active_Camera()->get_View_Matrix();
    glUniformMatrix4fv(glGetUniformLocation(ssaoShaderProgram->name, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glBindImageTexture(0, gPosition, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(1, gNormal, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glUniform3fv(glGetUniformLocation(ssaoShaderProgram->name, "kernel"), NUM_SSAO_SAMPLES, glm::value_ptr(ssaoKernel[0]));
    glm::vec2 resolution = glm::vec2(width, height);
    glUniform2fv(glGetUniformLocation(ssaoShaderProgram->name, "resolution"), 1, &resolution[0]);
    glUniform3fv(glGetUniformLocation(ssaoShaderProgram->name, "radiusBiasPower"), 1, &radiusBiasPower[0]);
    quad->draw();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::executeLightingPass(bool useSSAO) {
    lightingPassShaderProgram->use();
    glBindImageTexture(0, gPosition, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(1, gNormal, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(2, gAlbedoSpec, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(3, ssaoTexture, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_R16F);
    glBindImageTexture(4, lightingPassTexture, 0, GL_FALSE, 0,  GL_WRITE_ONLY, GL_RGBA16F);

    Camera* camera = Camera::get_Active_Camera();
    glm::vec3 cameraPos = camera->get_Camera_Position();
    glUniform3fv(glGetUniformLocation(lightingPassShaderProgram->name, "cameraPos"), 1, &cameraPos[0]);

    auto &lightSource = LightSource::getInstance();
    lightSource.bindToShader(lightingPassShaderProgram->name);
    lightSource.bindLightMatrices(lightingPassShaderProgram->name);
    lightSource.bindDepthMap();

    glUniform1i(glGetUniformLocation(lightingPassShaderProgram->name, "useSSAO"), useSSAO);

    quad->draw();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GBuffer::executeBloomPass() {
    const int NUM_PASSES = 5;
    for (int i = 0; i < NUM_PASSES; ++i) {
        bloomShaderProgram->use();
        GLuint readBuffer = (i % 2 == 0) ? lightingPassTexture : bloomBuffer;
        GLuint writeBuffer = (i % 2 == 1) ? lightingPassTexture : bloomBuffer;
        glBindImageTexture(0, readBuffer, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
        glBindImageTexture(1, writeBuffer, 0, GL_FALSE, 0,  GL_WRITE_ONLY, GL_RGBA16F);
        bool lastPass = i == NUM_PASSES - 1;
        glUniform1i(glGetUniformLocation(bloomShaderProgram->name, "lastPass"), lastPass);
        quad->draw();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}

GBuffer::Quad::Quad() {
    //https://learnopengl.com/Getting-started/Hello-Triangle
    float vertices[] = {
            // positions          // colors           // texture coords
            1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    GLuint VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void GBuffer::Quad::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GBuffer::prepareRefractionPass(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, refractionBuffer);
    /*
    if (width != allocatedWidth || height != allocatedHeight) {
        allocateTextures(width, height);
    }
    */

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

    glBindImageTexture(0, gPosition, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(1, gNormal, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
}

void GBuffer::finishRefractionPass() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
