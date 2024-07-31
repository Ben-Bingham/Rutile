#pragma once
#include "BVHIndex.h"
#include "BVHNode.h"

#include "Utility/Bank.h"

namespace Rutile {
    using SceneBVHBank = Bank<SceneBVHNode, BVHIndex>;
    using ObjectBVHBank = Bank<ObjectBVHNode, BVHIndex>;
}