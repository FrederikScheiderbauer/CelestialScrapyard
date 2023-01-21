#include "../headers/GBuffer.hpp"

GBuffer::GBuffer(int width, int height) {
    glGenTextures(1, &gBuffer);
    allocateTexture(width, height);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GBuffer::allocateTexture(int width, int height) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, gBuffer);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, NUM_LAYERS, 0, GL_RGBA, GL_FLOAT, nullptr);
    allocatedWidth = width;
    allocatedHeight = height;
}

void GBuffer::checkDimensions(int width, int height) {
    if (width != allocatedWidth || height != allocatedHeight) {
        allocateTexture(width, height);
    }
}

void GBuffer::bindToShader() {
    glBindImageTexture(0, gBuffer, 0, GL_TRUE, 0,  GL_READ_WRITE, GL_RGBA32F);
}

void GBuffer::clear() {
    glClearTexImage(gBuffer, 0, GL_RED, GL_FLOAT, nullptr);
}
