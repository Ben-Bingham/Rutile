#pragma once
#include "BVHIndex.h"

#include "Utility/RayTracing/AABB.h"

namespace Rutile {
    class BVHNode {
    public:
        AABB bbox;

        BVHIndex node1;
        BVHIndex node2;
    };
}