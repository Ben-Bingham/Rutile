#include "BVHFactory.h"
#include "BVHNode.h"
#include <algorithm>
#include <iostream>
#include <array>

#include "Settings/App.h"

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    BVHFactory::ReturnStructure BVHFactory::Construct(const Scene& scene) {
        TLASBank bank{ };

        if (scene.objects.empty()) {
            std::cout << "ERROR: Cannot create a BVH for a scene with no objects" << std::endl;
        }

        BVHIndex startingIndex = Construct(scene.objects, bank, scene);

        for (BVHIndex i = 0; i < (BVHIndex)bank.Size(); ++i) {
            bank[i].bbox.AddPadding(0.1f);
        }

        return ReturnStructure{ bank, startingIndex };
    }

    // There will always be at least 1 object
    BVHIndex BVHFactory::Construct(const std::vector<Object>& objects, TLASBank& bank, const Scene& scene) {
        TLASNode node;

        node.bbox = AABBFactory::Construct(objects);

        if (objects.size() == 1) {
            node.node1ObjIndex = -1;
            node.node2 = -1;

            std::vector<Object>::const_iterator it = std::find(objects.begin(), objects.end(), objects[0]);
            if (it != objects.end()) {
                for (size_t i = 0; i < scene.objects.size(); ++i) {
                    if (scene.objects[i] == *it) {
                        node.node1ObjIndex = (int)i;
                        break;
                    }
                }
            }

        } else if (objects.size() >= 2) {
            auto [group1, group2] = DivideObjects(objects, node.bbox, scene);

            node.node1ObjIndex = Construct(group1, bank, scene);
            node.node2 = Construct(group2, bank, scene);
        }

        return bank.Add(node);
    }

    float Area(const AABB& bbox) {
        glm::vec3 extent = bbox.max - bbox.min;
        return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
    }

    glm::vec3 Center(Object obj, const Scene& scene) {
        Geometry geo = scene.geometryBank[obj.geometry];
        Transform t = scene.transformBank[obj.transform];

        t.CalculateMatrix();

        glm::vec3 center{ 0.0f };
        float i = 0.0f;
        for (auto vert : geo.vertices) {
            center += glm::vec3{ t.matrix * glm::vec4{ vert.position, 1.0f } };
            i += 1.0f;
        }

        return center / i;
    }

    // Taken from: https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/
    float EvaluateSAH(int axis, float pos, const std::vector<Object>& objects, const Scene& scene) {
        AABB leftBox;
        AABB rightBox;
        int leftCount = 0, rightCount = 0;
        for (size_t i = 0; i < objects.size(); ++i) {
            Object obj = objects[i];
            glm::vec3 center = Center(obj, scene);

            if (center[axis] < pos) {
                leftCount++;
                leftBox = AABBFactory::Construct(AABBFactory::Construct(scene.geometryBank[obj.geometry], scene.transformBank[obj.transform]), leftBox);
            }
            else {
                rightCount++;
                rightBox = AABBFactory::Construct(AABBFactory::Construct(scene.geometryBank[obj.geometry], scene.transformBank[obj.transform]), rightBox);
            }
        }

        float a = Area(leftBox);
        float b = Area(rightBox);

        float cost = leftCount * Area(leftBox) + rightCount * Area(rightBox);

        if (cost < 0.0f) {
            int a = 1;
        }

        return cost > 0.0f ? cost : std::numeric_limits<float>::max();
    }

    std::pair<std::vector<Object>, std::vector<Object>> BVHFactory::DivideObjects(const std::vector<Object>& objects, const AABB& bbox, const Scene& scene) {
        std::vector<Object> group1{ };
        std::vector<Object> group2{ };

        int bestAxis = -1;
        float bestPos = 0;
        float bestCost = std::numeric_limits<float>::max();
        for (int axis = 0; axis < 3; ++axis) {
            for (size_t i = 0; i < objects.size(); ++i) {
                Object obj = objects[i];
                glm::vec3 center = Center(obj, scene);

                float candidatePos = center[axis];
                float cost = EvaluateSAH(axis, candidatePos, objects, scene);
                if (cost < bestCost) {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        int axis = bestAxis;
        float splitPos = bestPos;

        for (const auto& obj : objects) {
            glm::vec3 center = Center(obj, scene);

            if (center[axis] > splitPos) {
                group1.push_back(obj);
            }
            else {
                group2.push_back(obj);
            }
        }

        if (group1.empty() || group2.empty()) {
            group1.clear();
            group2.clear();

            glm::vec3 extent = bbox.max - bbox.min;
            axis = 0;
            if (extent.y > extent.x) {
                axis = 1;
            }
            if (extent.z > extent[axis]) {
                axis = 2;
            }

            std::vector<Object> objs = objects;

            std::sort(objs.begin(), objs.end(), [axis, scene](const Object& obj1, const Object& obj2) {
                glm::vec3 c1 = Center(obj1, scene);
                glm::vec3 c2 = Center(obj2, scene);

                return c1[axis] > c2[axis];
            });

            size_t midPoint = objects.size() / 2;

            group1.insert(group1.end(), objs.begin(), objs.begin() + midPoint);
            group2.insert(group2.end(), objs.begin() + midPoint, objs.end());
        }
        
        return std::make_pair(group1, group2);
    }

    BVHFactory::ReturnStructure2 BVHFactory::Construct(const Geometry& geometry, Transform transform) {
        std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
        std::cout << "Making BLAS for geometry with: " << geometry.indices.size() / 3 << " triangles" << std::endl;
        std::vector<Triangle> triangles;

        transform.CalculateMatrix();

        for (size_t i = 0; i < geometry.indices.size(); i += 3) {
            const Vertex v1 = geometry.vertices[geometry.indices[i + 0]];
            const Vertex v2 = geometry.vertices[geometry.indices[i + 1]];
            const Vertex v3 = geometry.vertices[geometry.indices[i + 2]];

            glm::vec4 p1 = transform.matrix * glm::vec4{ v1.position, 1.0 };
            glm::vec4 p2 = transform.matrix * glm::vec4{ v2.position, 1.0 };
            glm::vec4 p3 = transform.matrix * glm::vec4{ v3.position, 1.0 };

            triangles.push_back(Triangle{ p1, p2, p3 });
        }

        if (triangles.empty()) {
            std::cout << "ERROR: Cannot create a BVH for an object with no triangles" << std::endl;
        }

        std::vector<BLASNode> nodes{ };
        nodes.resize(1);

        const int rootNodeIndex = 0;
        BLASNode& rootNode = nodes[rootNodeIndex];
        rootNode.node1 = -1;

        rootNode.triangleOffset = 0;
        rootNode.triangleCount = (int)triangles.size();

        rootNode.bbox = AABBFactory::Construct(triangles);

        Subdivide(rootNodeIndex, nodes, triangles);

        for (auto& node : nodes) {
            node.bbox.AddPadding(0.01f);
        }

        std::cout << "Finished BVH creation, it took: " << (double)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count() / 1000000.0 << "ms" << std::endl;
        return ReturnStructure2{ nodes, triangles };
    }

    // Taken from: https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/
    float EvaluateSAH(int axis, float pos, const std::vector<Triangle>& triangles) {
        AABB leftBox;
        AABB rightBox;
        int leftCount = 0, rightCount = 0;
        for (const auto& tri : triangles) {
            glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;

            if (centroid[axis] < pos) {
                leftCount++;
                leftBox = AABBFactory::Construct(AABBFactory::Construct(tri), leftBox);
            }
            else {
                rightCount++;
                rightBox = AABBFactory::Construct(AABBFactory::Construct(tri), rightBox);
            }
        }

        float cost = leftCount * Area(leftBox) + rightCount * Area(rightBox);
        return cost > 0 ? cost : std::numeric_limits<float>::max();
    }

    // Taken from: https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
    void BVHFactory::Subdivide(int nodeIndex, std::vector<BLASNode>& nodes, std::vector<Triangle>& triangles) {
        BLASNode& node = nodes[nodeIndex];

        if (node.triangleCount <= 2) {
            return;
        }

        int bestAxis = -1;
        float bestPos = 0;
        float bestCost = std::numeric_limits<float>::max();
        for (int axis = 0; axis < 3; ++axis) {
            for (const auto& tri : triangles) {
                glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;

                const float candidatePos = centroid[axis];
                const float cost = EvaluateSAH(axis, candidatePos, triangles);

                if (cost < bestCost) {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        const int axis = bestAxis;
        const float splitPos = bestPos;

        // Split triangles, based on position
        int i = node.triangleOffset;
        int j = i + node.triangleCount - 1;
        while (i <= j) {
            Triangle& tri = triangles[i];
            glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;
            if (centroid[axis] < splitPos) {
                ++i;
            }
            else {
                std::swap(triangles[i], triangles[j--]);
            }
        }

        const int lessCount = i - node.triangleOffset;
        if (lessCount == 0 || lessCount == node.triangleCount) {
            return;
        }

        // Create child nodes
        const int node1Idx = (int)nodes.size();
        const int node2Idx = node1Idx + 1;

        nodes.resize(nodes.size() + 2);

        node.node1 = node1Idx;

        nodes[node1Idx].triangleOffset = node.triangleOffset;
        nodes[node1Idx].triangleCount = lessCount;

        nodes[node2Idx].triangleOffset = i;
        nodes[node2Idx].triangleCount = node.triangleCount - lessCount;

        node.triangleCount = 0;

        // Expand node AABBs
        BLASNode& node1 = nodes[node1Idx];
        std::vector<Triangle> node1Tris;
        node1Tris.insert(node1Tris.end(), triangles.begin() + node1.triangleOffset, triangles.begin() + node1.triangleOffset + node1.triangleCount);
        node1.bbox = AABBFactory::Construct(node1Tris);

        BLASNode& node2 = nodes[node2Idx];
        std::vector<Triangle> node2Tris;
        node2Tris.insert(node2Tris.end(), triangles.begin() + node2.triangleOffset, triangles.begin() + node2.triangleOffset + node2.triangleCount);
        node2.bbox = AABBFactory::Construct(node2Tris);

        // Recurse
        Subdivide(node1Idx, nodes, triangles);
        Subdivide(node2Idx, nodes, triangles);
    }
}