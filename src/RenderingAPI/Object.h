#pragma once
#include "RenderingBanks.h"
#include <string>

namespace Rutile {
    struct Object {
        std::string name;

        GeometryIndex geometry;
        MaterialIndex material;
        TransformIndex transform;

        friend bool operator==(const Object& lhs, const Object& rhs) {
            return lhs.name == rhs.name
                && lhs.geometry == rhs.geometry
                && lhs.material == rhs.material
                && lhs.transform == rhs.transform;
        }

        friend bool operator!=(const Object& lhs, const Object& rhs) { return !(lhs == rhs); }
    };
}