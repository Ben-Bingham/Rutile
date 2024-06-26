#pragma once
#include "rendering/Scene.h"

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
		Scene GetBundle(GeometryMode mode);

		void Add(Primitive primitive, glm::mat4* transform, MaterialType materialType, Material* material);

		void Add(LightType type, Light* light);

	private:
		Scene m_CurrentBundle;
	};
}