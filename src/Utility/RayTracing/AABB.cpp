#include "AABB.h"

namespace Rutile {
    void AABB::AddPadding(float padding) {
        min -= padding / 2.0f;
        max += padding / 2.0f;
    }
}