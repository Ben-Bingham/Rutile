#include "GeometryFactory.h"

#include <glm/ext/scalar_constants.hpp>

namespace Rutile {
    Geometry GeometryFactory::Construct(Primitive primitive) {
        Geometry geo;
        switch (primitive) {
        case Primitive::TRIANGLE:
            geo.name = "Triangle";

            geo.vertices = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.0f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.5f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            geo.indices = {
                2, 1, 0
            };

            geo.type = Geometry::GeometryType::TRIANGLE;

            break;

        case Primitive::CUBE:
            geo.name = "Cube";

            geo.vertices = {
                //      Position                         Normal                             Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f } },

                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 1.0f } },

                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },
            };

            geo.indices = {
                 2,  1,  0,
                 0,  3,  2,

                 4,  5,  6,
                 6,  7,  4,

                 8,  9, 10,
                10, 11,  8,

                14, 13, 12,
                12, 15, 14,

                16, 17, 18,
                18, 19, 16,

                22, 21, 20,
                20, 23, 22,
            };

            geo.type = Geometry::GeometryType::CUBE;

            break;
        case Primitive::SQUARE:
            geo.name = "Square";

            geo.vertices = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            geo.indices = {
                2, 1, 0,
                3, 2, 0
            };

            geo.type = Geometry::GeometryType::SQUARE;

            break;
        case Primitive::SPHERE:
            geo.name = "Sphere";

            std::vector<float> vertices;

            constexpr int numberOfStacks = 18;
            constexpr int numberOfSectors = 36;

            constexpr float sectorStep = 2 * glm::pi<float>() / numberOfSectors;
            constexpr float stackStep = glm::pi<float>() / numberOfStacks;

            for (unsigned int i = 0; i <= numberOfStacks; ++i) {
                const float stackAngle = glm::pi<float>() / 2 - i * stackStep; // goes from top to bottom

                for (unsigned int j = 0; j <= numberOfSectors; j++) {
                    const float sectorAngle = j * sectorStep;

                    Vertex vertex;

                    // Positions
                    vertex.position.x = cosf(sectorAngle) * cosf(stackAngle);
                    vertex.position.y = sinf(stackAngle);
                    vertex.position.z = sinf(sectorAngle) * cosf(stackAngle);

                    // Normals
                    vertex.normal.x = cosf(sectorAngle) * cosf(stackAngle);
                    vertex.normal.y = sinf(stackAngle);
                    vertex.normal.z = sinf(sectorAngle) * cosf(stackAngle);


                    // UVs
                    vertex.uv.x = (float)j / numberOfSectors;
                    vertex.uv.y = (float)i / numberOfStacks;

                    geo.vertices.push_back(vertex);
                }
            }

            for (unsigned int i = 0; i < numberOfStacks; i++) {
                unsigned int k1 = i * (numberOfSectors + 1);
                unsigned int k2 = k1 + numberOfSectors + 1;

                for (unsigned int j = 0; j < numberOfSectors; j++, k1++, k2++) {
                    if (i != 0) {
                        geo.indices.push_back(k1 + 1);
                        geo.indices.push_back(k2);
                        geo.indices.push_back(k1);
                    }

                    if (i != (numberOfStacks - 1)) {
                        geo.indices.push_back(k2 + 1);
                        geo.indices.push_back(k2);
                        geo.indices.push_back(k1 + 1);
                    }
                }
            }

            geo.type = Geometry::GeometryType::SPHERE;
        }

        return geo;
    }

    Geometry GeometryFactory::ConstructQuad(const glm::vec3& c1, const glm::vec3& c2, const glm::vec3& c3, const glm::vec3& c4) {
        Geometry geo;

        geo.name = "Square";

        glm::vec3 normal{ };

        const glm::vec3 a = c1 - c2;
        const glm::vec3 b = c3 - c2;

        normal.x = a.y * b.z - a.z * b.y;
        normal.y = a.z * b.x - a.x * b.z;
        normal.z = a.x * b.y - a.y * b.x;

        geo.vertices = {
            //      Position Normal       Uv
            Vertex{ c1,      normal, glm::vec2{ 0.0f, 0.0f } },
            Vertex{ c2,      normal, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ c3,      normal, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ c4,      normal, glm::vec2{ 1.0f, 0.0f } },
        };

        geo.indices = {
            2, 1, 0,
            3, 2, 0
        };

        geo.type = Geometry::GeometryType::SQUARE;

        return geo;
    }

    Geometry GeometryFactory::Construct(const AABB& aabb) {
        Geometry geo;

        geo.name = "Bounding Box";

        geo.vertices = {
            //      Position                         Normal                             Uv
            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.min.z }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.min.z }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.min.z }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.min.z }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f } },

            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.max.z }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.max.z }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.max.z }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.max.z }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 1.0f } },

            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.max.z }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.min.z }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.min.z }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.max.z }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.max.z }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.min.z }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.min.z }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.max.z }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.min.z }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.min.z }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.min.y, aabb.max.z }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.min.y, aabb.max.z }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.min.z }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.min.z }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{ aabb.max.x, aabb.max.y, aabb.max.z }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
            Vertex{ glm::vec3{ aabb.min.x, aabb.max.y, aabb.max.z }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },
        };

        geo.indices = {
             2,  1,  0,
             0,  3,  2,

             4,  5,  6,
             6,  7,  4,

             8,  9, 10,
            10, 11,  8,

            14, 13, 12,
            12, 15, 14,

            16, 17, 18,
            18, 19, 16,

            22, 21, 20,
            20, 23, 22,
        };

        geo.type = Geometry::GeometryType::CUBE;

        return geo;
    }
}