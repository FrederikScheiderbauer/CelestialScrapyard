#ifndef GGD_LIGHTSOURCE_HPP
#define GGD_LIGHTSOURCE_HPP


#include "glm/vec3.hpp"
#include "glad/glad.h"

class LightSource {
private:
    glm::vec3 position;
    const float LIGHT_SOURCE_SPEED = 0.005f;
    LightSource();
public:
    static LightSource& getInstance();
    void updatePosition();
    void bindToShader(GLuint program);
    LightSource(LightSource const&) = delete;
    void operator=(LightSource const&) = delete;
};


#endif //GGD_LIGHTSOURCE_HPP
