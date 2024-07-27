#pragma once
#include "BVHIndex.h"
#include "BVHNode.h"

#include "Utility/Bank.h"

namespace Rutile {
    using BVHBank = Bank<BVHNode, BVHIndex>;
}