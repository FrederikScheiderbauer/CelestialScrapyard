#ifndef GGD_FORWARDRENDER_HPP
#define GGD_FORWARDRENDER_HPP


#include "glad/glad.h"

class ForwardRender {
private:
    GLuint forwardFBO, forwardTexture, depthAndStencil;
    int allocatedWidth, allocatedHeight;
    void allocateTextures(int width, int height);
    void bindToFBO();
public:
    ForwardRender(int width, int height);
    void prepareForwardRender(int width, int height);
    void finishForwardRender();
    void executePostProcess();
};


#endif //GGD_FORWARDRENDER_HPP
