#pragma once
#include <map>

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
		Scene& GetScene();

		void Add(GeometryIndex geometry,			   TransformIndex transform,   MaterialIndex material,       const std::string& name = "");

		void Add(const Geometry& geometry,			   const Transform& transform, const Material& material,     const std::string& name = "");

		void Add(GeometryIndex geometry, const Transform& transform, const Material& material, const std::string& name = "");

		void Add(GeometryFactory::Primitive primitive, const Transform& transform, const Material& material,	 const std::string& name = "");
		void Add(const Geometry& geometry,			   const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		void Add(const Geometry& geometry,			   const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		void Add(const PointLight& pointLight);
		void Add(const DirectionalLight& light);

		void Add(const std::string& path, TransformIndex transform, MaterialIndex materialIndex);
		void Add(const std::string& path, const Transform& transform);
		void Add(const std::string& path, const Transform& transform, const Material& material);
		void Add(const std::string& path, const Transform& transform, MaterialIndex material);

	private:
		void LoadAssimpNode(const aiNode* node, const aiScene* scene, TransformIndex transform, MaterialIndex materialIndex = std::numeric_limits<size_t>::max());

		Scene m_Scene;

		int m_ObjectNamingIndex{ 0 };
	};
}