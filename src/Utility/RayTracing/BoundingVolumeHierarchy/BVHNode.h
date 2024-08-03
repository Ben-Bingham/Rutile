#pragma once
#include "BVHIndex.h"

#include "Utility/RayTracing/AABB.h"

namespace Rutile {
    struct TLASNode {
        AABB bbox;

        BVHIndex node1ObjIndex;
        BVHIndex node2;

        //int objIndex;
        int pad;
    };

    struct BLASNode {
        AABB bbox;

        BVHIndex node1;

        int triangleOffset;
        int triangleCount;
    };
}