#ifndef GGD_GBUFFER_HPP
#define GGD_GBUFFER_HPP


#include <memory>
#include "glad/glad.h"
#include "ShaderProgram.hpp"
#include "glm/vec3.hpp"
#include "Quad.hpp"

class GBuffer {
private:
    GLuint intermediateFBO;
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec, depthAndStencil;
    int allocatedWidth, allocatedHeight;
    const int NUM_LAYERS = 3;
    void allocateTextures(int width, int height);
    void allocateMultisampleTextures(int width, int height);
    void bindToFBO();
    void bindToFBOMultiSample();

    std::unique_ptr<Quad> quad;
    std::unique_ptr<ShaderProgram> lightingPassShaderProgram;

    std::unique_ptr<ShaderProgram> ssaoShaderProgram;
    static const int NUM_SSAO_SAMPLES = 64;
    glm::vec3 ssaoKernel[NUM_SSAO_SAMPLES];
    GLuint ssaoBuffer;
    GLuint ssaoTexture;

    std::unique_ptr<ShaderProgram> refractionShaderProgram;
    std::unique_ptr<ShaderProgram> reflectionShaderProgram;
    GLuint refractionBuffer;
    GLuint refractionTexture;
    GLuint reflectionBuffer;
    GLuint reflectionTexture;
    GLuint rPosition, rNormal, rAlbedoSpec;
    void allocateAndBindReflectionTextures(int width, int height);

    void generateRefractionBuffer(int width, int height);

public:
    GBuffer(int width, int height);
    void prepareGeometryPass(int width, int height, bool is_MSAA_enabled);
    void finishGemoetryPass();
    void finishGemoetryPassMultisample();
    void executeSSAOPass(int width, int height, glm::vec3 &radiusBiasPower);
    void executeSSAOPassMultisample(int width, int height, glm::vec3 &radiusBiasPower);
    void executeLightingPass(bool useSSAO);
    void prepareRefractionPass(int width, int height);
    void finishRefractionPass();
    void blitDepthAndStencilBuffer(GLuint forwardBuffer);
};


#endif //GGD_GBUFFER_HPP
