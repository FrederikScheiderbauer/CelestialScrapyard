#ifndef GGD_GBUFFER_HPP
#define GGD_GBUFFER_HPP


#include <memory>
#include "glad/glad.h"
#include "ShaderProgram.hpp"

class GBuffer {
private:
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec, depthAndStencil;
    int allocatedWidth, allocatedHeight;
    const int NUM_LAYERS = 3;
    void allocateTextures(int width, int height);
    void bindToFBO();

    class Quad
    {
    private:
        GLuint VAO;
    public:
        Quad();
        void draw();
    };
    std::unique_ptr<Quad> quad;
    std::unique_ptr<ShaderProgram> lightingPassShaderProgram;
public:
    GBuffer(int width, int height);
    void prepareGeometryPass(int width, int height);
    void finishGemoetryPass();
    void executeLightingPass();
    void blitDepthAndStencilBuffer();
};


#endif //GGD_GBUFFER_HPP
