#pragma once
#include <glm/glm.hpp>

namespace Rutile {
    struct Camera {
        glm::vec3 position   { 0.0f,  0.0f,  7.5f };
        glm::vec3 frontVector{ 0.0f,  0.0f, -1.0f };
        glm::vec3 upVector   { 0.0f,  1.0f,  0.0f };
        glm::vec3 rightVector{ 1.0f,  0.0f,  0.0f };

        float speed = 5.0f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float lookSensitivity = 0.25f;

        glm::mat4 View() const;
    };
}