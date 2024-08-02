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
            SceneBVHBank bank;
            BVHIndex startingIndex;

            std::vector<float> triangles;
            
            struct ObjectTriangleData {
                int triangleOffset;
                int triangleCount;
            };

            std::vector<ObjectTriangleData> objTriData;
        };

        struct ReturnStructure2 {
            ObjectBVHBank bank;
            BVHIndex startingIndex;

            std::vector<Triangle> triangles;
        };

        // Scene BVH
        static ReturnStructure Construct(const Scene& scene);

    private:
        static BVHIndex Construct(const std::vector<Object>& objects, SceneBVHBank& bank, const Scene& scene);

        static std::pair<std::vector<Object>, std::vector<Object>> DivideObjects(const std::vector<Object>& objects, const AABB& bbox, const Scene& scene);

    public:
        // Object BVH
        static ReturnStructure2 Construct(const Geometry& geometry, Transform transform);

    private:
        static inline size_t m_MaxDepth = 32;

        static BVHIndex Construct(const std::vector<Triangle>& triangles, ObjectBVHBank& bank, size_t depth, int offset, std::vector<Triangle>& finalTriangles);

        static std::pair<std::vector<Triangle>, std::vector<Triangle>> DivideTriangles(const std::vector<Triangle>& triangles, const AABB& bbox);
    };
}