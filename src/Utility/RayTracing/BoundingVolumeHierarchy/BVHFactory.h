#pragma once
#include "BVHBank.h"

#include "RenderingAPI/Scene.h"

#include "Utility/RayTracing/AABB.h"
#include "Utility/RayTracing/Triangle.h"

#include <iostream>

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    class BVHUtility {
    public:
        static glm::vec3 Center(const Triangle& triangle);
        static glm::vec3 Center(const Object& object);
        static glm::vec3 Center(const AABB& bbox);
    };

    template<typename T>
    class TemplateBVHFactory {
    public:
        struct Node {
            AABB bbox;

            int node1;

            int offset;
            int count;
        };

        static std::vector<Node> Construct(std::vector<T>& objects, int minObjectsPerNode = 2) {
            std::vector<Node> nodes{ };
            nodes.resize(1);

            if (objects.empty()) {
                std::cout << "ERROR: cannot create a BVH with no objects" << std::endl;
            }

            constexpr int rootNodeIndex = 0;
            Node& rootNode = nodes[rootNodeIndex];
            rootNode.node1 = -1;

            rootNode.offset = 0;
            rootNode.count = (int)objects.size();

            rootNode.bbox = AABBFactory::Construct(objects);

            Subdivide(rootNodeIndex, nodes, objects, minObjectsPerNode);

            for (auto& node : nodes) {
                node.bbox.AddPadding(0.01f);
            }

            return nodes;
        }

    private:
        static float Area(const AABB& bbox) {
            glm::vec3 extent = bbox.max - bbox.min;
            return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
        }

        // Taken from: https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/
        static float EvaluateSAH(int axis, float pos, const std::vector<T>& objects) {
            AABB leftBox;
            AABB rightBox;
            int leftCount = 0, rightCount = 0;
            for (const auto& obj : objects) {
                glm::vec3 center = BVHUtility::Center(obj);

                if (center[axis] < pos) {
                    leftCount++;
                    leftBox = AABBFactory::Construct(AABBFactory::Construct(obj), leftBox);
                }
                else {
                    rightCount++;
                    rightBox = AABBFactory::Construct(AABBFactory::Construct(obj), rightBox);
                }
            }

            const float cost = (float)leftCount * Area(leftBox) + (float)rightCount * Area(rightBox);
            return cost > 0 ? cost : std::numeric_limits<float>::max();
        }

        static float FindBestSplitPlane(Node& node, int& bestAxis, float& bestPosition, const std::vector<T>& objects) {
            float bestCost = std::numeric_limits<float>::max();

            for (int axis = 0; axis < 3; ++axis) {
                const float minBound = node.bbox.min[axis];
                const float maxBound = node.bbox.max[axis];

                if (minBound == maxBound) {
                    continue;
                }

                constexpr int divisions = 8;
                const float scale = (maxBound - minBound) / (float)divisions;

                for (int i = 0; i < divisions; ++i) {
                    const float candidatePos = minBound + (float)(i * scale);
                    const float cost = EvaluateSAH(axis, candidatePos, objects);

                    if (cost < bestCost) {
                        bestPosition = candidatePos;
                        bestAxis = axis;
                        bestCost = cost;
                    }
                }
            }

            return bestCost;
        }

        static float CalculateNodeCost(const Node& node) {
            const glm::vec3 extent = node.bbox.max - node.bbox.min;
            const float surfaceArea = extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;

            return surfaceArea * node.count;
        }

        static void Subdivide(int nodeIndex, std::vector<Node>& nodes, std::vector<T>& objects, int minObjectsPerNode) {
            if (nodes[nodeIndex].count <= minObjectsPerNode) {
                return;
            }

            std::vector<T> objs;
            objs.insert(objs.end(), objects.begin() + nodes[nodeIndex].offset, objects.begin() + nodes[nodeIndex].offset + nodes[nodeIndex].count);
            int axis;
            float splitPos;

            const float cost = FindBestSplitPlane(nodes[nodeIndex], axis, splitPos, objs);
            const float currentCostNoSplit = CalculateNodeCost(nodes[nodeIndex]);

            if (currentCostNoSplit < cost) {
                return;
            }

            // Split objects, based on position
            int i = nodes[nodeIndex].offset;
            int j = i + nodes[nodeIndex].count - 1;
            while (i <= j) {
                T& obj = objects[i];

                glm::vec3 center = BVHUtility::Center(obj);

                if (center[axis] < splitPos) {
                    ++i;
                }
                else {
                    std::swap(objects[i], objects[j--]);
                }
            }

            const int lessCount = i - nodes[nodeIndex].offset;
            if (lessCount == 0 || lessCount == nodes[nodeIndex].count) {
                return;
            }

            // Create child nodes
            const int node1Idx = (int)nodes.size();
            const int node2Idx = node1Idx + 1;

            nodes.resize(nodes.size() + 2);

            nodes[nodeIndex].node1 = node1Idx;

            nodes[node1Idx].offset = nodes[nodeIndex].offset;
            nodes[node1Idx].count = lessCount;

            nodes[node2Idx].offset = i;
            nodes[node2Idx].count = nodes[nodeIndex].count - lessCount;

            nodes[nodeIndex].count = 0;

            // Expand node AABBs
            Node& node1 = nodes[node1Idx];
            std::vector<T> node1Objs;
            node1Objs.insert(node1Objs.end(), objects.begin() + node1.offset, objects.begin() + node1.offset + node1.count);
            node1.bbox = AABBFactory::Construct(node1Objs);

            Node& node2 = nodes[node2Idx];
            std::vector<T> node2Objs;
            node2Objs.insert(node2Objs.end(), objects.begin() + node2.offset, objects.begin() + node2.offset + node2.count);
            node2.bbox = AABBFactory::Construct(node2Objs);

            // Recurse
            Subdivide(node1Idx, nodes, objects, minObjectsPerNode);
            Subdivide(node2Idx, nodes, objects, minObjectsPerNode);
        }
    };
}