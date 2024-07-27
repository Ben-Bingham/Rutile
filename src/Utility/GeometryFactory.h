#pragma once
#include "RayTracing/AABB.h"

#include "RenderingAPI/Geometry.h"

namespace Rutile {
    class GeometryFactory {
    public:
        enum class Primitive {
            TRIANGLE,
            SQUARE,
            CUBE,
            SPHERE
        };

        static Geometry Construct(Primitive primitive);
        static Geometry ConstructQuad(const glm::vec3& c1, const glm::vec3& c2, const glm::vec3& c3, const glm::vec3& c4);
        static Geometry Construct(const AABB& aabb);
    };
}