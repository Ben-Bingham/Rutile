#pragma once
#include <vector>

#include <glm/glm.hpp>

namespace Rutile {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;
    };

    using Index = uint32_t;

    enum class MaterialType {
        FLAT,
        COLOR,
        PHONG
    };

    struct Packet {
        std::vector<Vertex> vertexData;
        std::vector<Index> indexData;

        MaterialType highestSupportedMaterialType;

        // std::shared_ptr<Texture> texture; // Or maybe an index
    };
}