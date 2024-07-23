#pragma once
#include "RenderingBanks.h"
#include <string>

namespace Rutile {
    struct Object {
        std::string name;

        GeometryIndex geometry;
        MaterialIndex material;
        TransformIndex transform;
    };
}