#pragma once
#include "Geometry.h"
#include "Transform.h"
#include "Material.h"

#include "Utility/Bank.h"

namespace Rutile {
    using GeometryIndex = size_t;
    using TransformIndex = size_t;
    using MaterialIndex = size_t;

    using GeometryBank = Bank<Geometry, GeometryIndex>;
    using TransformBank = Bank<Transform, TransformIndex>;
    using MaterialBank = Bank<Material, MaterialIndex>;
}