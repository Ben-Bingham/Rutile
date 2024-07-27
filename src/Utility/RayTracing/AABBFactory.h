#pragma once
#include "AABB.h"

#include "RenderingAPI/Geometry.h"
#include "RenderingAPI/Object.h"
#include "RenderingAPI/Transform.h"

namespace Rutile {
    class AABBFactory {
    public:
        static AABB Construct(const AABB& bbox1, const AABB& bbox2);
        static AABB Construct(const Geometry& geometry, Transform transform);
        static AABB Construct(const std::array<glm::vec3, 3>& triangle);
        static AABB Construct(const std::vector<Object>& objects);
    };
}