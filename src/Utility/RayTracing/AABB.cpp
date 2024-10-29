#include "AABB.h"

namespace Rutile {
    void AABB::AddPadding(float padding) {
        min -= padding / 2.0f;
        max += padding / 2.0f;
    }

    bool AABB::Contains(const glm::vec3& point) {
        const bool withinX = point.x > min.x && point.x < max.x;
        const bool withinY = point.y > min.y && point.y < max.y;
        const bool withinZ = point.z > min.z && point.z < max.z;

        return withinX && withinY && withinZ;
    }
}