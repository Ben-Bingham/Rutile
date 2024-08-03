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

    void Subdivide(int nodeIndex, std::vector<BLASNode>& nodes, std::vector<Triangle>& triangles, int& nodesUsed);

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

        std::vector<BLASNode> nodes;

        nodes.resize(2 * triangles.size() - 1);

        const int rootNodeIndex = 0;
        BLASNode& rootNode = nodes[rootNodeIndex];
        rootNode.node1 = -1;

        rootNode.triangleOffset = 0;
        rootNode.triangleCount = (int)triangles.size();

        rootNode.bbox = AABBFactory::Construct(triangles);

        int nodesUsed = 1;
        Subdivide(rootNodeIndex, nodes, triangles, nodesUsed); // TODO replace nodes used with expanding nodes vector

        for (BVHIndex i = 0; i < (BVHIndex)nodes.size(); ++i) {
            nodes[i].bbox.AddPadding(0.1f);
        }

        std::cout << "Finished BVH creation, it took: " << (double)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count() / 1000000.0 << "ms" << std::endl;
        return ReturnStructure2{ nodes, triangles };
    }

    // Taken from: https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/
    float EvaluateSAH(int axis, float pos, const std::vector<Triangle>& triangles) {
        AABB leftBox;
        AABB rightBox;
        int leftCount = 0, rightCount = 0;
        for (size_t i = 0; i < triangles.size(); ++i) {
            Triangle tri = triangles[i];
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
    void Subdivide(int nodeIndex, std::vector<BLASNode>& nodes, std::vector<Triangle>& triangles, int& nodesUsed) {
        BLASNode& node = nodes[nodeIndex];

        if (node.triangleCount <= 2) {
            return;
        }

        int bestAxis = -1;
        float bestPos = 0;
        float bestCost = std::numeric_limits<float>::max();
        for (int axis = 0; axis < 3; ++axis) {
            for (size_t i = 0; i < triangles.size(); ++i) {
                Triangle tri = triangles[i];
                glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;

                float candidatePos = centroid[axis];
                float cost = EvaluateSAH(axis, candidatePos, triangles);
                if (cost < bestCost) {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        int axis = bestAxis;
        float splitPos = bestPos;

        // Split triangles, based on position
        int i = node.triangleOffset;
        int j = i + node.triangleCount - 1;
        while (i <= j) {
            Triangle& tri = triangles[i];
            glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;
            if (centroid[axis] < splitPos) {
                ++i;
            }
            else
                std::swap(triangles[i], triangles[j--]);
        }

        int leftCount = i - node.triangleOffset;
        if (leftCount == 0 || leftCount == node.triangleCount) {
            return;
        }

        // Create child nodes
        int leftChildIdx = nodesUsed++;
        int rightChildIdx = nodesUsed++;

        node.node1 = leftChildIdx;

        nodes[leftChildIdx].triangleOffset = node.triangleOffset;
        nodes[leftChildIdx].triangleCount = leftCount;

        nodes[rightChildIdx].triangleOffset = i;
        nodes[rightChildIdx].triangleCount = node.triangleCount - leftCount;

        node.triangleCount = 0;

        // Expand node AABBs
        BLASNode& node1 = nodes[leftChildIdx];
        std::vector<Triangle> node1Tris;
        node1Tris.insert(node1Tris.end(), triangles.begin() + node1.triangleOffset, triangles.begin() + node1.triangleOffset + node1.triangleCount);
        node1.bbox = AABBFactory::Construct(node1Tris);

        BLASNode& node2 = nodes[rightChildIdx];
        std::vector<Triangle> node2Tris;
        node2Tris.insert(node2Tris.end(), triangles.begin() + node2.triangleOffset, triangles.begin() + node2.triangleOffset + node2.triangleCount);
        node2.bbox = AABBFactory::Construct(node2Tris);

        // Recurse
        Subdivide(leftChildIdx, nodes, triangles, nodesUsed);
        Subdivide(rightChildIdx, nodes, triangles, nodesUsed);
    }

    BVHIndex BVHFactory::Construct(const std::vector<Triangle>& triangles, BLASBank& bank, size_t depth, int offset, std::vector<Triangle>& finalTriangles) {
        BLASNode node;

        node.bbox = AABBFactory::Construct(triangles);

        if (depth > m_MaxDepth || triangles.size() <= 3) {
            // Leaf node (stores all remaining triangles)
            node.node1 = -1;

            node.triangleCount = (int)triangles.size();
            node.triangleOffset = (int)finalTriangles.size();

            finalTriangles.insert(finalTriangles.end(), triangles.begin(), triangles.end());

        } else {
            // Branch node (stores 2 nodes as children)
            auto [group1, group2] = DivideTriangles(triangles, node.bbox);

            int node1Offset = (int)finalTriangles.size();

            depth += 1;

            node.triangleCount = 0;
            node.triangleOffset = -1;

            node.node1 = Construct(group1, bank, depth, node1Offset, finalTriangles);
            //node.node2 = Construct(group2, bank, depth, node1Offset + (int)group2.size(), finalTriangles);
        }

        return bank.Add(node);
    }

    std::pair<std::vector<Triangle>, std::vector<Triangle>> BVHFactory::DivideTriangles(const std::vector<Triangle>& triangles, const AABB& bbox) {
        std::vector<Triangle> group1{ };
        std::vector<Triangle> group2{ };

        int bestAxis = -1;
        float bestPos = 0;
        float bestCost = std::numeric_limits<float>::max();
        for (int axis = 0; axis < 3; ++axis) {
            for (size_t i = 0; i < triangles.size(); ++i) {
                Triangle tri = triangles[i];
                glm::vec3 centroid = (tri[0] + tri[1] + tri[2]) / 3.0f;

                float candidatePos = centroid[axis];
                float cost = EvaluateSAH(axis, candidatePos, triangles);
                if (cost < bestCost) {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        int axis = bestAxis;
        float splitPos = bestPos;

        for (const auto& tri : triangles) {
            glm::vec3 pos = tri[0] + tri[1] + tri[2];
            pos /= 3.0f;

            if (pos[axis] > splitPos) {
                group1.push_back(tri);
            } else {
                group2.push_back(tri);
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

            std::vector<Triangle> tris = triangles;

            std::sort(tris.begin(), tris.end(), [axis] (const Triangle& t1, const Triangle& t2) {
                glm::vec3 c1 = (t1[0] + t1[1] + t1[2]) / 3.0f;
                glm::vec3 c2 = (t2[0] + t2[1] + t2[2]) / 3.0f;

                return c1[axis] > c2[axis];
            });

            size_t midPoint = triangles.size() / 2;

            group1.insert(group1.end(), tris.begin(), tris.begin() + midPoint);
            group2.insert(group2.end(), tris.begin() + midPoint, tris.end());
        }

        return std::make_pair(group1, group2);
    }
}