#pragma once
#include <assimp/scene.h>

#include "RenderingAPI/Light.h"
#include "RenderingAPI/Material.h"
#include "RenderingAPI/RenderingBanks.h"
#include "RenderingAPI/Scene.h"
#include "RenderingAPI/Transform.h"

#include "Utility/GeometryFactory.h"
#include "Utility/MaterialFactory.h"

namespace Rutile {
	class SceneFactory {
	public:
		Scene GetScene();

		void Add(GeometryIndex geometry,			   TransformIndex transform,   MaterialIndex material,       const std::string& name = "");

		void Add(const Geometry& geometry,			   const Transform& transform, const Material& material,     const std::string& name = "");

		void Add(GeometryFactory::Primitive primitive, const Transform& transform, const Material& material,	 const std::string& name = "");
		void Add(const Geometry& geometry,			   const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		void Add(const Geometry& geometry,			   const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		void Add(const PointLight& pointLight);
		void Add(const DirectionalLight& light);

		void Add(const std::string& path, TransformIndex transform);
		void Add(const std::string& path, const Transform& transform);

	private:
		void LoadAssimpNode(const aiNode* node, const aiScene* scene, TransformIndex transform);

		Scene m_CurrentScene;

		int m_ObjectNamingIndex{ 0 };
	};
}