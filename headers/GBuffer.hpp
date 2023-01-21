#ifndef GGD_GBUFFER_HPP
#define GGD_GBUFFER_HPP


#include "glad/glad.h"

class GBuffer {
private:
    GLuint gBuffer;
    int allocatedWidth, allocatedHeight;
    const int NUM_LAYERS = 3;
    void allocateTexture(int width, int height);
public:
    GBuffer(int width, int height);
    void checkDimensions(int width, int height);
    void bindToShader();
    void clear();
};


#endif //GGD_GBUFFER_HPP
