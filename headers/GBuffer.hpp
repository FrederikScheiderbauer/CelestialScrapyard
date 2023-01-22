#ifndef GGD_GBUFFER_HPP
#define GGD_GBUFFER_HPP


#include "glad/glad.h"

class GBuffer {
private:
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec, depthAndStencil;
    int allocatedWidth, allocatedHeight;
    const int NUM_LAYERS = 3;
    void allocateTextures(int width, int height);
    void bindToFBO();
public:
    GBuffer(int width, int height);
    void prepareRender(int width, int height);
    void finishRender();
    void bindToShader();
    void blitDepthAndStencilBuffer();
};


#endif //GGD_GBUFFER_HPP
