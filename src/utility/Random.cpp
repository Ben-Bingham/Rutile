#include "Random.h"
#include <random>

#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>

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
        // This function was taken from:
        //https://github.com/riccardoprosdocimi/real-time-ray-tracer/blob/master/shaders/frag.glsl
        const glm::vec3 randomVector = RandomVec3();
        const float phi = 2.0f * glm::pi<float>() * randomVector.x;
        const float cosTheta = 2.0f * randomVector.y - 1.0f;
        const float u = randomVector.z;
        
        const float theta = acos(cosTheta);
        const float r = pow(u, 1.0f / 3.0f);
        
        const float x = r * sin(theta) * cos(phi);
        const float y = r * sin(theta) * sin(phi);
        const float z = r * cos(theta);
        
        return { x, y, z };
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