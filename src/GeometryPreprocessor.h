#pragma once
#include "Bundle.h"

namespace Rutile {
    enum class GeometryMode {
        OPTIMIZED,
		NO_INDICES
    };

    enum class Primitive {
        TRIANGLE,
		SQUARE,
		CUBE
    };

	class GeometryPreprocessor {
	public:
		Bundle GetBundle(GeometryMode mode);

		void Add(Primitive primitive, glm::mat4 transform, glm::vec3 color);

	private:
		Bundle m_CurrentBundle;
	};
}