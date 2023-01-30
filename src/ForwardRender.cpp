#include <iostream>
#include "../headers/ForwardRender.hpp"

ForwardRender::ForwardRender(int width, int height) {
    glGenFramebuffers(1, &forwardFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);
    glGenTextures(1, &forwardTexture);
    glGenTextures(1, &depthAndStencil);
    allocateTextures(width, height);
    bindToFBO();
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "incomplete forward buffer" << std::endl;
    }
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

    allocatedWidth = width;
    allocatedHeight = height;
}

void ForwardRender::bindToFBO() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, forwardTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAndStencil, 0);
}

void ForwardRender::prepareForwardRender(int width, int height) {
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

void ForwardRender::executePostProcess() {

}
