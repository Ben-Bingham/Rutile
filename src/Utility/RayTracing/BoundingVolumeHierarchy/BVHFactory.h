#pragma once
#include "BVHBank.h"

#include "RenderingAPI/Scene.h"

namespace Rutile {
    class BVHFactory {
    public:
        struct ReturnStructure {
            BVHBank bank;
            BVHIndex startingIndex;

            std::vector<float> triangles;
            
            struct ObjectTriangleData {
                int triangleOffset;
                int triangleCount;
            };

            std::vector<ObjectTriangleData> objTriData;
        };

        static ReturnStructure Construct(const Scene& scene);

    private:
        static BVHIndex Construct(const std::vector<Object>& objects, BVHBank& bank, const Scene& scene, size_t depth);

        static std::pair<std::vector<Object>, std::vector<Object>> DivideObjects(const std::vector<Object>& objects);

        static inline size_t m_MaxDepth = 32;
    };
}