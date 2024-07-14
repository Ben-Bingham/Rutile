#include "Random.h"
#include <random>

#include <glm/ext/quaternion_geometric.hpp>

namespace Rutile {
    float RandomFloat() {
        return RandomFloat(0.0f, 1.0f);
    }

    float RandomFloat(float min, float max) {
        static std::uniform_real_distribution distribution(min, max);
        static std::mt19937 generator{ };
        return distribution(generator);
    }

    glm::vec3 RandomVec3() {
        return glm::vec3{ RandomFloat(), RandomFloat(), RandomFloat() };
    }

    glm::vec3 RandomVec3(float min, float max) {
        return glm::vec3{ RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) };
    }

    glm::vec3 RandomVec3InUnitSphere() {
        while (true) {
            glm::vec3 trial = RandomVec3(-1.0f, 1.0f);
            if (glm::length(trial) < 1.0f) {
                return trial;
            }
        }
    }

    glm::vec3 RandomUnitVec3() {
        return glm::normalize(RandomVec3InUnitSphere());
    }

    glm::vec3 RandomVec3InHemisphere(glm::vec3 normal) {
        const glm::vec3 unitSphere = RandomUnitVec3();
        if (dot(unitSphere, normal) > 0.0) { // In the same hemisphere as the normal
            return unitSphere;
        }
        else {
            return -unitSphere;
        }
    }
}