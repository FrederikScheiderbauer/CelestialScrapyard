#include "../headers/OrbitUtil.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "../headers/Random.hpp"

glm::vec3 getRandomPositionInOrbit(float lowerRadius, float upperRadius) {
    float x_pos = Random::getInRange(-1.f, 1.f);
    float z_pos = Random::getInRange(-1.f, 1.f);
    float r = Random::getInRange(lowerRadius, upperRadius);

    return r * glm::normalize(glm::vec4(x_pos, 0.f, z_pos, 0.f));
}

glm::vec3 cartesianToSpherical(glm::vec3 center) {
    //https://mathworld.wolfram.com/SphericalCoordinates.html
    float r = glm::length(glm::vec3(center));
    float phi = glm::acos(center.z / r);
    float theta = glm::atan(center.y, center.x);
    return {r, 0.5f * glm::pi<float>() - phi, theta};
}

glm::vec3 sphericalToCartesian(glm::vec3 spherical) {
    //https://mathworld.wolfram.com/SphericalCoordinates.html
    float x = spherical.x * glm::cos(spherical.z) * glm::cos(spherical.y);
    float y = spherical.x * glm::sin(spherical.z) * glm::cos(spherical.y);
    float z = spherical.x * glm::sin(spherical.y);
    return {x, y, z};
}

glm::vec3 doOrbitalMovement(glm::vec3 &spherical, float speed) {
    float signedSpeed = speed * (glm::abs(spherical.z) < 0.5f * glm::pi<float>() ? 1.f : -1.f);
    spherical.y += signedSpeed;
    // spherical.y += signedSpeed* (glm::length(spherical) / 2); // adjust speed of object depending on distance to planet
    spherical.y = std::fmod(spherical.y, glm::pi<float>());
    return sphericalToCartesian(spherical);
}
//This function makes it so an Object in Obrit
glm::vec3 doOrbitalMovementForAsteroids(glm::vec3 &spherical, float speed) {
    float signedSpeed = speed * (glm::abs(spherical.z) < 0.5f * glm::pi<float>() ? 1.f : -1.f);
    //spherical.y += signedSpeed;
    float distance_to_center = glm::length(spherical);
    float orbit_speed = 4/distance_to_center;
    spherical.y += signedSpeed; // adjust speed of object depending on distance to planet
    spherical.y = std::fmod(spherical.y, glm::pi<float>());
    return sphericalToCartesian(spherical);
}

glm::vec3 moveInOrbit(glm::vec3 position, float speed, bool isAsteroid) {
    auto spherical = cartesianToSpherical(position);

    if (!isAsteroid) {
    return doOrbitalMovement(spherical, speed);
    } else {
        float distance_to_center = glm::length(position);
        float orbit_speed = 4/distance_to_center;
        return doOrbitalMovementForAsteroids(spherical, speed *orbit_speed);
    }
}

glm::vec3 moveInOrbitWithTheta(glm::vec3 position, float speed, float &theta) {
    auto spherical = cartesianToSpherical(position);
    spherical.z = theta;
    glm::vec3 result = doOrbitalMovement(spherical, speed);
    theta = spherical.z;
    return result;
}
