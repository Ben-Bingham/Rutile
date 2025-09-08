#pragma once
#include "AABB.h"
#include "Triangle.h"

#include "Utility/Transform.h"
#include "Scene/Mesh.h"
#include "Scene/Object.h"

namespace Rutile {
    class AABBFactory {
    public:
        static AABB Construct(const AABB& bbox1, const AABB& bbox2);
        static AABB Construct(const Mesh& mesh, glm::mat4 transform);
        static AABB Construct(const Triangle& triangle);
        static AABB Construct(const Object& object);
        static AABB Construct(const std::vector<Object>& objects);
        static AABB Construct(const std::vector<Triangle>& triangles);
    };
}