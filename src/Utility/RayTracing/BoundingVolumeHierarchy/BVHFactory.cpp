#include "BVHFactory.h"
#include <algorithm>
#include <iostream>

#include "Settings/App.h"

#include "Utility/RayTracing/AABBFactory.h"

namespace Rutile {
    std::pair<BVHBank, BVHIndex> BVHFactory::Construct(const Scene& scene) {
        BVHBank bank{ };

        if (scene.objects.empty()) {
            std::cout << "ERROR: Cannot create a BVH for a scene with on objects" << std::endl;
        }

        BVHIndex startingIndex = Construct(scene.objects, bank);

        for (BVHIndex i = 0; i < (BVHIndex)bank.Size(); ++i) {
            bank[i].bbox.AddPadding(0.1f);
        }

        return std::make_pair(bank, startingIndex);
    }

    // There will always be at least 1 object
    BVHIndex BVHFactory::Construct(const std::vector<Object>& objects, BVHBank& bank) {
        BVHNode node;

        node.bbox = AABBFactory::Construct(objects);
        node.objectIndex = -1;

        if (objects.size() == 1) {
            node.node1 = std::numeric_limits<BVHIndex>::max();
            node.node2 = std::numeric_limits<BVHIndex>::max();

            std::vector<Object>::const_iterator it = std::find(objects.begin(), objects.end(), objects[0]);
            if (it != objects.end()) {
                for (size_t i = 0; i < App::scene.objects.size(); ++i) {
                    if (App::scene.objects[i] == *it) {
                        node.objectIndex = (int)i;
                        break;
                    }
                }
            }

        } else if (objects.size() >= 2) {
            auto [group1, group2] = DivideObjects(objects);

            node.node1 = Construct(group1, bank);
            node.node2 = Construct(group2, bank);
        }

        return bank.Add(node);
    }

    std::pair<std::vector<Object>, std::vector<Object>> BVHFactory::DivideObjects(const std::vector<Object>& objects) { // TODO
        std::vector<Object> group1;
        std::vector<Object> group2;

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
            if (App::transformBank[object.transform].position.x < smallestX) {
                smallestX = App::transformBank[object.transform].position.x;
            }
        }

        std::vector<float> closestDistances;

        for (size_t i = 0; i < objects.size(); ++ i) {
            float distanceToAxis = App::transformBank[objects[i].transform].position.x - smallestX;

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
}