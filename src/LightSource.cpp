#include "../headers/LightSource.hpp"
#include "../headers/OrbitUtil.hpp"

LightSource::LightSource() {
    position = getRandomPositionInOrbit(10.f, 10.1f);
}

void LightSource::updatePosition() {
    position = moveInOrbit(position, LIGHT_SOURCE_SPEED);
}

void LightSource::bindToShader(GLuint program) {
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &position[0]);
}

LightSource& LightSource::getInstance() {
    static LightSource instance;
    return instance;
}
