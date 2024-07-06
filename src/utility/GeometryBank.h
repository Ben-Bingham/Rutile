#pragma once
#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace Rutile {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    using Index = uint32_t;

    struct Geometry {
        std::vector<Vertex> vertices;
        std::vector<Index> indices;
    };

    using GeometryIndex = size_t;

    class GeometryBank {
    public:
        GeometryBank() = default;

        GeometryIndex Add(std::string& name, const Geometry& geometry);

        Geometry& operator[](GeometryIndex i);

        std::string GetName(GeometryIndex i);

        size_t Size();

    private:
        std::vector<Geometry> m_Geometries;

        std::vector<std::string> m_Names;
    };
}