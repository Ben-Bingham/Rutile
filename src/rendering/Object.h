#pragma once
#include "utility/GeometryBank.h"
#include "utility/MaterialBank.h"
#include "utility/TransformBank.h"

namespace Rutile {
    struct Object {
        std::string name;

        GeometryIndex geometry;
        MaterialIndex material;
        TransformIndex transform;
    };
}