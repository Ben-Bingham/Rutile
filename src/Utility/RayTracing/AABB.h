#pragma once
#include <glm/vec3.hpp>

namespace Rutile {
    struct AABB {
        glm::vec3 min{ std::numeric_limits<float>::max() };
        glm::vec3 max{ -std::numeric_limits<float>::max() };
    };
}