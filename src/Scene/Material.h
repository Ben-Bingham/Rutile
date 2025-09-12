#pragma once

#include <glm/glm.hpp>

#include "Utility/Image.h"

namespace Rutile {
	struct Material {
        glm::vec3 diffuse;
        glm::vec3 ambient;
        glm::vec3 specular;

        float shininess;

        enum class Type {
            DIFFUSE,
            MIRROR,
            DIELECTRIC,
            EMISSIVE,
            ONE_WAY_MIRROR
        } type{ Type::DIFFUSE };

        float fuzz;

        float indexOfRefraction;
	};
}