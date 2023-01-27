#ifndef GGD_LIGHTSOURCE_HPP
#define GGD_LIGHTSOURCE_HPP


#include "glm/glm.hpp"
#include "glad/glad.h"

class LightSource {
private:
    glm::vec3 position;
    GLuint depthMapFBO;
    GLuint depthMap;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 1024;
    const float LIGHT_SOURCE_SPEED = 0.0005f;
    LightSource();
public:
    static LightSource& getInstance();
    void updatePosition();
    void bindToShader(GLuint program);
    void prepareDepthMapCreation();
    void finishDepthMapCreation(int width, int height);
    void bindLightMatrices(GLuint program);
    void bindDepthMap();
    GLuint getDepthMap();
    glm::vec3& getPosition();

    LightSource(LightSource const&) = delete;
    void operator=(LightSource const&) = delete;
};


#endif //GGD_LIGHTSOURCE_HPP
