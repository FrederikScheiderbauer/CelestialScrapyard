#ifndef GGD_NOISE_HPP
#define GGD_NOISE_HPP

#include "PerlinNoise/PerlinNoise.hpp"
#include <glm/glm.hpp>

class Noise {
public:
    enum Type {
        SIMPLE, AMPLIFIED
    };
    struct Parameters {
        Type type;
        float strength;
        float persistence;
        float amplification;
    };
    constexpr static Parameters asteroid = {SIMPLE, 1.f, 0.5f, 1.f};
    constexpr static Parameters mountainous = {AMPLIFIED, 0.8f, 0.25f, 1.5f};
    Noise(unsigned long seed, Noise::Parameters parameters);
    float getValue(glm::vec3 point);
private:
    siv::PerlinNoise perlin;
    Parameters parameters;
};
#endif
