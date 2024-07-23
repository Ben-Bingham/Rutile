#pragma once

#include "Bank.h"

#include "Geometry.h"
#include "rendering/Material.h"
#include "rendering/Transform.h"

namespace Rutile {
    using GeometryIndex = size_t;
    using TransformIndex = size_t;
    using MaterialIndex = size_t;

    using GeometryBank = Bank<Geometry, GeometryIndex>;
    using TransformBank = Bank<Transform, TransformIndex>;
    using MaterialBank = Bank<Material, MaterialIndex>;
}