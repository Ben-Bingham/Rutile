#pragma once
#include <vector>

namespace Rutile {
    struct Vertex {
        //glm::vec3
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