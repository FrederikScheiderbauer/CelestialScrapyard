#ifndef GGD_FORWARDRENDER_HPP
#define GGD_FORWARDRENDER_HPP


#include "glad/glad.h"
#include "Quad.hpp"
#include "ShaderProgram.hpp"

class ForwardRender {
private:
    GLuint forwardFBO, forwardTexture, depthAndStencil;
    int allocatedWidth, allocatedHeight;
    std::unique_ptr<Quad> quad;
    void allocateTextures(int width, int height);
    void bindToFBO();
    std::unique_ptr<ShaderProgram> bloomShaderProgram;
    GLuint bloomBuffer;
public:
    ForwardRender(int width, int height);
    void prepareForwardRender(int width, int height);
    void finishForwardRender();
    void executePostProcess();
    GLuint getFBO();
};


#endif //GGD_FORWARDRENDER_HPP
