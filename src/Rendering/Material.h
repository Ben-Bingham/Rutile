#pragma once
#include <string>

#include <glm/vec3.hpp>

namespace Rutile {
    enum class MaterialType {
        SOLID,
        PHONG
    };

    struct Material {
        std::string name;

        struct Solid {
            glm::vec3 color;
        } solid;

        struct Phong {
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            float shininess;
        } phong;
    };
}