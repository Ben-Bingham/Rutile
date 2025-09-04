#pragma once
#include <vector>

#include <glm/glm.hpp>

namespace Rutile {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    using Index = uint32_t;

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<Index> indices;
    };
}