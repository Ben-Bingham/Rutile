#pragma once
#include "BVHIndex.h"

#include "Utility/RayTracing/AABB.h"

namespace Rutile {
    struct TLASNode {
        AABB bbox;

        //BVHIndex node1ObjIndex;
        BVHIndex node1;
        BVHIndex node2;

        int objIndex;
    };

    struct BLASNode {
        AABB bbox;

        BVHIndex node1;
        BVHIndex node2;

        int triangleOffset;
        int triangleCount;
    };
}