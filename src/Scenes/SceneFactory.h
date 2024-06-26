#pragma once
#include "Scenes/Scene.h"

namespace Rutile {
    enum class Primitive {
        TRIANGLE,
		SQUARE,
		CUBE
    };

	class SceneFactory {
	public:
		Scene GetScene();

		void Add(Primitive primitive, Transform* transform, MaterialType materialType, Material* material);

		void Add(LightType type, Light* light);

	private:
		Scene m_CurrentScene;
	};
}