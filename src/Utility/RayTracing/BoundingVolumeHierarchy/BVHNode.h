#pragma once
#include "BVHIndex.h"

#include "Utility/RayTracing/AABB.h"

namespace Rutile {
    struct SceneBVHNode {
        AABB bbox;

        BVHIndex node1;
        BVHIndex node2;

        int objectIndex{ -1 };
    };

    struct ObjectBVHNode {
        AABB bbox;

        BVHIndex node1;
        BVHIndex node2;

        int triangleOffset;
        int triangleCount;
    };
}