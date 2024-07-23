#pragma once
#include "Geometry.h"

namespace Rutile {
    class GeometryFactory {
    public:
        enum class Primitive {
            TRIANGLE,
            SQUARE,
            CUBE,
            SPHERE
        };
            
        static Geometry Construct(Primitive primitive);
    };
}