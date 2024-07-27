#pragma once
#include "BVHBank.h"

#include "RenderingAPI/Scene.h"

namespace Rutile {
    class BVHFactory {
    public:
        static std::pair<BVHBank, BVHIndex> Construct(const Scene& scene);
        static BVHIndex Construct(const std::vector<Object>& objects, BVHBank& bank);

    private:
        static std::pair<std::vector<Object>, std::vector<Object>> DivideObjects(const std::vector<Object>& objects);
    };
}