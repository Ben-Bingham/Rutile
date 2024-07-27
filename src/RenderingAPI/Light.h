#pragma once
#include <glm/vec3.hpp>

namespace Rutile {
    struct DirectionalLight {
        glm::vec3 direction = { -1.0f, -1.0f, -1.0f };

        glm::vec3 ambient =  { 0.2f, 0.2f, 0.2f };
        glm::vec3 diffuse =  { 0.5f, 0.5f, 0.5f };
        glm::vec3 specular = { 0.1f, 0.1f, 0.1f };
    };

    struct PointLight {
        glm::vec3 position;

        float constant  = 0.3f;
        float linear    = 0.1f;
        float quadratic = 0.03f;

        glm::vec3 ambient  = { 0.2f, 0.2f, 0.2f };
        glm::vec3 diffuse  = { 0.5f, 0.5f, 0.5f };
        glm::vec3 specular = { 0.1f, 0.1f, 0.1f };

        float shadowMapNearPlane = 1.0f;
        float shadowMapFarPlane  = 25.0f;
    };
}