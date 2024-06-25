#pragma once
#include "rendering/Bundle.h"

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

		void Add(Primitive primitive, glm::mat4 transform, MaterialType materialType, Material* material);

		void Add(LightType type, Light* light);

	private:
		Bundle m_CurrentBundle;
	};
}