#pragma once
#include <glm/vec3.hpp>

namespace Rutile {
    float RandomFloat(); // Generates a number in the range [0.0, 1.0)
    float RandomFloat(float min, float max); // Generates a number in the range [min, max)

    glm::vec3 RandomVec3();
    glm::vec3 RandomVec3(float min, float max);

    glm::vec3 RandomVec3InUnitSphere();
    glm::vec3 RandomUnitVec3(); // Returns a normalized vec3 on the surface of the unit sphere
    glm::vec3 RandomVec3InHemisphere(glm::vec3 normal);
}