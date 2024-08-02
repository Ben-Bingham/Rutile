#pragma once
#include "BVHIndex.h"
#include "BVHNode.h"

#include "Utility/Bank.h"

namespace Rutile {
    using TLASBank = Bank<TLASNode, BVHIndex>;
    using BLASBank = Bank<BLASNode, BVHIndex>;
}