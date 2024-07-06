#pragma once
#include "rendering/Transform.h"

namespace Rutile {
    using TransformIndex = size_t;

    class TransformBank {
    public:
        TransformBank() = default;

		TransformIndex Add(const Transform& transform);

        Transform& operator[](TransformIndex i);

	private:
		std::vector<Transform> m_Transforms;
    };
}