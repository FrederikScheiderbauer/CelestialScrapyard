#include <iostream>
#include "../headers/ForwardRender.hpp"
#include "config/config.h"

const std::vector<std::string> BLOOM_SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/quad.vert", (std::string)Project_SOURCE_DIR + "/src/shader/bloom.frag"};
const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

ForwardRender::ForwardRender(int width, int height) {
    bloomShaderProgram = std::make_unique<ShaderProgram>(BLOOM_SHADER_PATHS, SHADER_TYPES);
    glGenFramebuffers(1, &forwardFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);
    glGenTextures(1, &forwardTexture);
    glGenTextures(1, &depthAndStencil);
    glGenTextures(1, &bloomBuffer);
    allocateTextures(width, height);
    bindToFBO();
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "incomplete forward buffer" << std::endl;
    }
    quad = std::make_unique<Quad>();
}


void ForwardRender::allocateTextures(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, forwardTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, depthAndStencil);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    allocatedWidth = width;
    allocatedHeight = height;
}

void ForwardRender::bindToFBO() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, forwardTexture, 0);
    GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAndStencil, 0);
}

void ForwardRender::prepareForwardRender(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);
    if (width != allocatedWidth || height != allocatedHeight) {
        allocateTextures(width, height);
    }

    bindToFBO();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void ForwardRender::finishForwardRender() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//bloom based on: https://learnopengl.com/Advanced-Lighting/Bloom
void ForwardRender::executePostProcess() {
    const int NUM_PASSES = 10;
    for (int i = 0; i < NUM_PASSES; ++i) {
        bloomShaderProgram->use();
        GLuint readBuffer = (i % 2 == 0) ? forwardTexture : bloomBuffer;
        GLuint writeBuffer = (i % 2 == 1) ? forwardTexture : bloomBuffer;
        glBindImageTexture(0, readBuffer, 0, GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA16F);
        glBindImageTexture(1, writeBuffer, 0, GL_FALSE, 0,  GL_WRITE_ONLY, GL_RGBA16F);
        bool lastPass = i == NUM_PASSES - 1;
        glUniform1i(glGetUniformLocation(bloomShaderProgram->name, "lastPass"), lastPass);
        quad->draw();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
}

GLuint ForwardRender::getFBO() {
    return forwardFBO;
}
