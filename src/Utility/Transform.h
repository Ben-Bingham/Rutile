#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Rutile {
    struct Transform {
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::quat rotation{ 0.0f, 0.0f, 0.0f, 0.0f };

        glm::mat4 matrix{ 1.0f };

        void CalculateMatrix();
    };
}