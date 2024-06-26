#pragma once
#include "Transform.h"
#include <vector>

#include <glm/glm.hpp>

#include "rendering/Material.h"

namespace Rutile {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    using Index = uint32_t;

    struct Packet {
        std::vector<Vertex> vertexData;
        std::vector<Index> indexData;

        MaterialType materialType;
        Material* material;

        Transform* transform;
    };
}