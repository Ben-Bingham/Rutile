#include "BVHFactory.h"
#include "BVHNode.h"
#include <algorithm>
#include <iostream>
#include <array>

#include "Settings/App.h"

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    glm::vec3 BVHUtility::Center(const Triangle& triangle) {
        return (triangle[0] + triangle[1] + triangle[2]) / 3.0f;
    }

    glm::vec3 BVHUtility::Center(const Object& object) {
        Geometry& geo = App::scene.geometryBank[object.geometry];
        Transform& t = App::scene.transformBank[object.transform];

        AABB bbox = AABBFactory::Construct(geo, t);

        return Center(bbox);
    }

    glm::vec3 BVHUtility::Center(const AABB& bbox) {
        return (bbox.min + bbox.max) / 2.0f;
    }
}