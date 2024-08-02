#include "BVHFactory.h"
#include "BVHNode.h"
#include <algorithm>
#include <iostream>
#include <array>

#include "Settings/App.h"

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    BVHFactory::ReturnStructure BVHFactory::Construct(const Scene& scene) {
        SceneBVHBank bank{ };

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
    BVHIndex BVHFactory::Construct(const std::vector<Object>& objects, SceneBVHBank& bank, const Scene& scene) {
        SceneBVHNode node;

        node.bbox = AABBFactory::Construct(objects);
        node.objectIndex = -1;

        if (objects.size() == 1) {
            node.node1 = std::numeric_limits<BVHIndex>::max();
            node.node2 = std::numeric_limits<BVHIndex>::max();

            std::vector<Object>::const_iterator it = std::find(objects.begin(), objects.end(), objects[0]);
            if (it != objects.end()) {
                for (size_t i = 0; i < scene.objects.size(); ++i) {
                    if (scene.objects[i] == *it) {
                        node.objectIndex = (int)i;
                        break;
                    }
                }
            }

        } else if (objects.size() >= 2) {
            auto [group1, group2] = DivideObjects(objects, node.bbox, scene);

            node.node1 = Construct(group1, bank, scene);
            node.node2 = Construct(group2, bank, scene);
        }

        return bank.Add(node);
    }

    std::pair<std::vector<Object>, std::vector<Object>> BVHFactory::DivideObjects(const std::vector<Object>& objects, const AABB& bbox, const Scene& scene) {
        std::vector<Object> group1{ };
        std::vector<Object> group2{ };

        //int axis = 0;
        //
        //glm::vec3 center = bbox.max - bbox.min;
        //
        //for (const auto& object : objects) {
        //    glm::vec3 pos = scene.transformBank[object.transform].position;

        //    if (pos.x > center.x) {
        //        group1.push_back(object);
        //    } else {
        //        group2.push_back(object);
        //    }
        //}

        /*size_t j = 0;
        for (size_t i = 0; i < (size_t)std::floor((float)objects.size() / 2.0f); ++i) {
            group1.push_back(objects[i]);
            ++j;
        }

        while (j < objects.size()) {
            group2.push_back(objects[j]);
            ++j;
        }*/
        
        size_t group1Size;
        size_t group2Size;

        if (objects.size() % 2 == 0) {
            group1Size = objects.size() / 2;
            group2Size = objects.size() / 2;
        } else {
            group1Size = (size_t)std::floor((float)objects.size() / 2.0f);
            group2Size = group1Size + 1;
        }

        float smallestX = std::numeric_limits<float>::max();

        for (auto object : objects) {
            if (App::scene.transformBank[object.transform].position.x < smallestX) {
                smallestX = App::scene.transformBank[object.transform].position.x;
            }
        }

        std::vector<float> closestDistances;

        for (size_t i = 0; i < objects.size(); ++ i) {
            float distanceToAxis = App::scene.transformBank[objects[i].transform].position.x - smallestX;

            closestDistances.push_back(distanceToAxis);
        }

        std::vector<std::pair<Object, float>> newObjects;

        int i = 0;
        for (auto distance : closestDistances) {
            newObjects.push_back(std::make_pair(objects[i], distance));
            ++i;
        }

        std::sort(newObjects.begin(), newObjects.end(), [](std::pair<Object, float> obj1, std::pair<Object, float> obj2) {
            return obj1.second < obj2.second;
        });

        std::vector<Object> finalObjects;

        for (auto obj : newObjects) {
            finalObjects.push_back(obj.first);
        }

        for (size_t i = 0; i < finalObjects.size(); ++i) {
            if (i < group1Size) {
                group1.push_back(finalObjects[i]);
            }
            else {
                group2.push_back(finalObjects[i]);
            }
        }
        
        return std::make_pair(group1, group2);
    }

    BVHFactory::ReturnStructure2 BVHFactory::Construct(const Geometry& geometry, Transform transform) {
        ObjectBVHBank bank{ };

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

        std::vector<Triangle> tris;
        BVHIndex startingIndex = Construct(triangles, bank, 0, 0, tris);

        for (BVHIndex i = 0; i < (BVHIndex)bank.Size(); ++i) {
            bank[i].bbox.AddPadding(0.1f);
        }

        return ReturnStructure2{ bank, startingIndex, tris };
    }

    BVHIndex BVHFactory::Construct(const std::vector<Triangle>& triangles, ObjectBVHBank& bank, size_t depth, int offset, std::vector<Triangle>& finalTriangles) {
        ObjectBVHNode node;

        node.bbox = AABBFactory::Construct(triangles);

        std::cout << triangles.size() << std::endl;

        if (depth > m_MaxDepth || triangles.size() <= 3) {

            // Leaf node (stores all remaining triangles)
            node.node1 = -1;
            node.node2 = -1;

            node.triangleCount = (int)triangles.size();
            node.triangleOffset = finalTriangles.size();

            finalTriangles.insert(finalTriangles.end(), triangles.begin(), triangles.end());

        } else {
            // Branch node (stores 2 nodes as children
            auto [group1, group2] = DivideTriangles(triangles, node.bbox);

            int node1Offset = finalTriangles.size();

            depth += 1;

            node.triangleCount = 0;
            node.triangleOffset = -1;

            node.node1 = Construct(group1, bank, depth, node1Offset, finalTriangles);
            node.node2 = Construct(group2, bank, depth, node1Offset + (int)group2.size(), finalTriangles);
        }

        return bank.Add(node);
    }

    float Area(const AABB& bbox) {
        glm::vec3 extent = bbox.max - bbox.min;
        return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
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

        float a = Area(leftBox);
        float a2 = Area(rightBox);

        float cost = leftCount * Area(leftBox) + rightCount * Area(rightBox);
        return cost > 0 ? cost : std::numeric_limits<float>::max();
    }

    std::pair<std::vector<Triangle>, std::vector<Triangle>> BVHFactory::DivideTriangles(const std::vector<Triangle>& triangles, const AABB& bbox) {
        std::vector<Triangle> group1{ };
        std::vector<Triangle> group2{ };

        int bestAxis = -1;
        float bestPos = 0;
        int bestCost = std::numeric_limits<int>::max();
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

            group1.insert(group1.end(), triangles.begin(), triangles.begin() + midPoint);
            group2.insert(group2.end(), triangles.begin() + midPoint, triangles.end());
        }

        return std::make_pair(group1, group2);
    }
}