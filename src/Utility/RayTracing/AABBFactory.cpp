#include "AABBFactory.h"

#include <array>
#include <iostream>

#include "Settings/App.h"

namespace Rutile {
    AABB AABBFactory::Construct(const AABB& bbox1, const AABB& bbox2) {
        glm::vec3 newMin{ std::numeric_limits<float>::max() };
        glm::vec3 newMax{ -std::numeric_limits<float>::max() };

        // Min
        if (bbox1.min.x < bbox2.min.x) {
            newMin.x = bbox1.min.x;
        } else {
            newMin.x = bbox2.min.x;
        }

        if (bbox1.min.y < bbox2.min.y) {
            newMin.y = bbox1.min.y;
        }
        else {
            newMin.y = bbox2.min.y;
        }

        if (bbox1.min.z < bbox2.min.z) {
            newMin.z = bbox1.min.z;
        }
        else {
            newMin.z = bbox2.min.z;
        }

        // Max
        if (bbox1.max.x > bbox2.max.x) {
            newMax.x = bbox1.max.x;
        }
        else {
            newMax.x = bbox2.max.x;
        }

        if (bbox1.max.y > bbox2.max.y) {
            newMax.y = bbox1.max.y;
        }
        else {
            newMax.y = bbox2.max.y;
        }

        if (bbox1.max.z > bbox2.max.z) {
            newMax.z = bbox1.max.z;
        }
        else {
            newMax.z = bbox2.max.z;
        }

        return AABB{ newMin, newMax };
    }

    AABB AABBFactory::Construct(const Geometry& geometry, Transform transform) {
        if (geometry.type == Geometry::GeometryType::SPHERE) {
            const glm::vec3 sphereCenter = transform.position;
            const glm::vec3 radius = transform.scale;

            return AABB{ sphereCenter - radius, sphereCenter + radius };
        }

        const std::vector<Vertex> vertices = geometry.vertices;
        const std::vector<Index> indices = geometry.indices;

        std::vector<Triangle> triangles;

        transform.CalculateMatrix();

        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 p1 = glm::vec3{ transform.matrix * glm::vec4{ vertices[indices[i + 0]].position, 1.0f } };
            glm::vec3 p2 = glm::vec3{ transform.matrix * glm::vec4{ vertices[indices[i + 1]].position, 1.0f } };
            glm::vec3 p3 = glm::vec3{ transform.matrix * glm::vec4{ vertices[indices[i + 2]].position, 1.0f } };

            triangles.push_back(Triangle{ p1, p2, p3 });
        }

        AABB mainBbox = Construct(triangles[0]);

        for (size_t i = 1; i < triangles.size(); ++i) {
            AABB bbox = Construct(triangles[i]);

            mainBbox = Construct(mainBbox, bbox);
        }

        return mainBbox;
    }

    AABB AABBFactory::Construct(const Triangle& triangle) {
        glm::vec3 min{ std::numeric_limits<float>::max() };
        glm::vec3 max{ -std::numeric_limits<float>::max() };

        for (const glm::vec3& point : triangle) {
            if (point.x < min.x) {
                min.x = point.x;
            }
            if (point.y < min.y) {
                min.y = point.y;
            }
            if (point.z < min.z) {
                min.z = point.z;
            }

            if (point.x > max.x) {
                max.x = point.x;
            }
            if (point.y > max.y) {
                max.y = point.y;
            }
            if (point.z > max.z) {
                max.z = point.z;
            }
        }

        return AABB{ min, max };
    }

    AABB AABBFactory::Construct(const Object& object) {
        return Construct(App::scene.geometryBank[object.geometry], App::scene.transformBank[object.transform]);
    }

    AABB AABBFactory::Construct(const std::vector<Object>& objects) {
        AABB mainBbox{ };

        for (auto object : objects) {
            AABB bbox = Construct(object);

            mainBbox = Construct(mainBbox, bbox);
        }

        return mainBbox;
    }

    AABB AABBFactory::Construct(const std::vector<Triangle>& triangles) {
        AABB mainBbox{ };

        for (auto triangle : triangles) {
            AABB bbox = Construct(triangle);

            mainBbox = Construct(mainBbox, bbox);
        }

        return mainBbox;
    }
}