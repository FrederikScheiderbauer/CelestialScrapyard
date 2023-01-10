#ifndef GGD_NOISE_HPP
#define GGD_NOISE_HPP

#include "PerlinNoise/PerlinNoise.hpp"
#include <glm/glm.hpp>

class Noise {
public:
    enum Type {
        SIMPLE, AMPLIFIED, MULTILAYER
    };
    struct Parameters {
        Type type;
        float strength;
        float persistence;
        float amplification;
        //external use
        float particleHeight;
        glm::vec3 planet_info;
    };
    constexpr static Parameters asteroid = {SIMPLE, 2.f, 0.5f, 2.f};
    constexpr static Parameters mountainous = {AMPLIFIED, 0.8f, 0.25f, 1.5f, 1.5f, {1.4f,1.7f,1.78f}};
    constexpr static Parameters forests = {AMPLIFIED, 0.8f, 0.25f, 1.5f};
    constexpr static Parameters continental = {MULTILAYER, 0.8f, 0.25f, 1.5f, 1.f, {1.105f,1.163f,1.269f}};
    Noise(unsigned long seed, Noise::Parameters parameters);
    float getValue(glm::vec3 point);
    float getParticleHeight();
    glm::vec3 getPlanetInfo();
private:
    siv::PerlinNoise perlin;
    Parameters parameters;
    const float SECOND_LAYER_OFFSET = 2.f;
};
#endif
