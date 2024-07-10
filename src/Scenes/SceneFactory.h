#pragma once
#include "Scenes/Scene.h"

namespace Rutile {
    enum class Primitive {
        TRIANGLE,
		SQUARE,
		CUBE,
		SPHERE
    };

	class SceneFactory {
	public:
		Scene GetScene();

		MaterialIndex Add(const std::string& objectName, Primitive primitive, const Transform& transform, const std::string& materialName, const Solid& solid, const Phong& phong);

		MaterialIndex Add(const std::string& objectName, Primitive primitive, const Transform& transform, MaterialIndex material);

		void Add(const PointLight& pointLight);
		void Add(const DirectionalLight& light);

	private:
		std::pair<Geometry, std::string> GetGeometry(Primitive primitive);

		Scene m_CurrentScene;
	};
}