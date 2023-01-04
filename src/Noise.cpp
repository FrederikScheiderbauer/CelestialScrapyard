#include "../headers/Noise.hpp"

Noise::Noise(unsigned long seed, Noise::Parameters parameters) {
    siv::PerlinNoise::seed_type iseed;
    if (seed == 0UL) {
        iseed = 123456u;
    } else {
        //take lowest 32 bits of seed
        iseed = seed;
    }
    this->perlin = siv::PerlinNoise(iseed);
    this->parameters = parameters;
}

float Noise::getValue(glm::vec3 point) {
    switch (this->parameters.type) {
        case SIMPLE: {
            float value = perlin.normalizedOctave3D_01(point.x, point.y, point.z, 8, parameters.persistence);
            //map from [0,1] to [0.6, 1.4]
            //value = (0.8f * value) + 0.6;
            value = glm::pow(value, parameters.amplification);
            return value;
        }
        case AMPLIFIED: {
            //based on: https://www.youtube.com/watch?v=H4g-TC__cvg
            float value = 1.0f - glm::abs(perlin.normalizedOctave3D(point.x, point.y, point.z, 8, parameters.persistence));
            //make peaks more pronounced
            value = glm::pow(value, parameters.amplification);
            value *= parameters.strength;
            return value;
        }
        case MULTILAYER: {
            float firstLayerValue = perlin.normalizedOctave3D_01(point.x, point.y, point.z, 8, 0.33f);
            firstLayerValue *= 0.8f;
            //float secondLayerValue = 1.0f - glm::abs(perlin.normalizedOctave3D(point.x, point.y, point.z, 8, parameters.persistence));
            //secondLayerValue = glm::pow(secondLayerValue, parameters.amplification);
            //secondLayerValue *= 0.25f;
            return glm::max(0.f, firstLayerValue /*+ (secondLayerValue * firstLayerValue)*/ - 0.4f);
        }
        default:
            return -1.f;
    }
}
