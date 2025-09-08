#include "BVHFactory.h"
#include "BVHNode.h"
#include <algorithm>
#include <iostream>
#include <array>

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    glm::vec3 BVHUtility::Center(const Triangle& triangle) {
        return (triangle[0] + triangle[1] + triangle[2]) / 3.0f;
    }

    glm::vec3 BVHUtility::Center(const Object& object) {
        Mesh mesh = object.mesh;
        glm::mat4 t = object.transform;

        AABB bbox = AABBFactory::Construct(mesh, t);

        return Center(bbox);
    }

    glm::vec3 BVHUtility::Center(const AABB& bbox) {
        return (bbox.min + bbox.max) / 2.0f;
    }
}