#pragma once
#include "BVHBank.h"

#include "RenderingAPI/Scene.h"

#include "Utility/RayTracing/AABB.h"
#include "Utility/RayTracing/Triangle.h"

namespace Rutile {
    class BVHFactory {
    public:
        // TODO get rid of theses or clean them up a lot
        struct ReturnStructure {
            TLASBank bank;
            BVHIndex startingIndex;

            std::vector<float> triangles;
            
            struct ObjectTriangleData {
                int triangleOffset;
                int triangleCount;
            };

            std::vector<ObjectTriangleData> objTriData;
        };

        struct ReturnStructure2 {
            std::vector<BLASNode> nodes;
            std::vector<Triangle> triangles;
        };

        // TLAS BVH
        static ReturnStructure Construct(const Scene& scene);

    private:
        static BVHIndex Construct(const std::vector<Object>& objects, TLASBank& bank, const Scene& scene);

        static std::pair<std::vector<Object>, std::vector<Object>> DivideObjects(const std::vector<Object>& objects, const AABB& bbox, const Scene& scene);

    public:
        // BLAS BVH
        static ReturnStructure2 Construct(const Geometry& geometry, Transform transform);

    private:
        static float FindBestSplitPlane(BLASNode& node, int& bestAxis, float& bestPosition, const std::vector<Triangle>& triangles);
        static float CalculateNodeCost(int nodeIndex, const std::vector<BLASNode>& nodes);

        static void Subdivide(int nodeIndex, std::vector<BLASNode>& nodes, std::vector<Triangle>& triangles);
    };
}