#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace Rutile {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    using Index = uint32_t;

    struct Geometry {
        std::string name;

        std::vector<Vertex> vertices;
        std::vector<Index> indices;

        enum class GeometryType {
            TRIANGLE,
            SQUARE,
            CUBE,
            SPHERE,
            MODEL
        };

        GeometryType type;
    };
}