#include "TransformBank.h"

namespace Rutile {
    TransformIndex TransformBank::Add(const Transform& transform) {
        m_Transforms.push_back(transform);

        return m_Transforms.size() - 1;
    }

    Transform& TransformBank::operator[](TransformIndex i) {
        return m_Transforms[i];
    }
}