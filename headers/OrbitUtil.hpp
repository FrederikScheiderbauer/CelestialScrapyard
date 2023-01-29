#ifndef GGD_ORBITUTIL_HPP
#define GGD_ORBITUTIL_HPP

#include "glm/glm.hpp"

glm::vec3 getRandomPositionInOrbit(float lowerRadius, float upperRadius);
glm::vec3 moveInOrbit(glm::vec3 position, float speed, bool isAsteroid);
glm::vec3 moveInOrbitWithTheta(glm::vec3 position, float speed, float &theta);
glm::vec3 cartesianToSpherical(glm::vec3 center);

#endif //GGD_ORBITUTIL_HPP
