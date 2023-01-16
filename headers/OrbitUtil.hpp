#ifndef GGD_ORBITUTIL_HPP
#define GGD_ORBITUTIL_HPP

#include "glm/glm.hpp"

glm::vec3 getRandomPositionInOrbit(float lowerRadius, float upperRadius);
glm::vec3 moveInOrbit(glm::vec3 position, float speed);

#endif //GGD_ORBITUTIL_HPP
