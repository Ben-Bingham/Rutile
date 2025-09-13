#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Scene/Material.h"
#include "Scene/Mesh.h"

namespace Rutile {
    struct Obj { // TODO find a better way to do this, this class should not be nesacary
        glm::mat4 transform;

        int mat;
        int mesh;

        static inline std::vector<Mesh> m_Meshs{ };
        static inline std::vector<Material> m_Materials{ };
    };
}
