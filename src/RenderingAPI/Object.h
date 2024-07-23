#pragma once
#include <string>

#include "utility/RenderingBanks.h"

namespace Rutile {
    struct Object {
        std::string name;

        GeometryIndex geometry;
        MaterialIndex material;
        TransformIndex transform;
    };
}