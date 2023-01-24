#ifndef GGD_GBUFFER_HPP
#define GGD_GBUFFER_HPP


#include <memory>
#include "glad/glad.h"
#include "ShaderProgram.hpp"
#include "glm/vec3.hpp"

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

    std::unique_ptr<ShaderProgram> ssaoShaderProgram;
    static const int NUM_SSAO_SAMPLES = 64;
    glm::vec3 ssaoKernel[NUM_SSAO_SAMPLES];
    GLuint ssaoBuffer;
    GLuint ssaoTexture;
public:
    GBuffer(int width, int height);
    void prepareGeometryPass(int width, int height);
    void finishGemoetryPass();
    void executeSSAOPass(int width, int height);
    void executeLightingPass();
    void blitDepthAndStencilBuffer();
};


#endif //GGD_GBUFFER_HPP
