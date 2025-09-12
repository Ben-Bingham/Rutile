#pragma once
#include <map>

#include <assimp/scene.h>

#include "Utility/Transform.h"

#include "GeometryFactory.h"
#include "MaterialFactory.h"

#include "Scene/Scene.h"

namespace Rutile {
	class SceneFactory {
	public:
		Scene& GetScene();

		void Add(Mesh mesh, glm::mat4 transform, Material material);
		void Add(Mesh mesh, Transform transform, Material material);

		void Add(GeometryFactory::Primitive primitive, Transform transform, Material material);

		//void Add(const Geometry& geometry,			   const Transform& transform, const Material& material,     const std::string& name = "");

		//void Add(GeometryIndex geometry, const Transform& transform, const Material& material, const std::string& name = "");

		//void Add(const Geometry& geometry,			   const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		//void Add(const Geometry& geometry,			   const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		//void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialFactory::Color color, const std::string& name = "");
		//void Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialIndex material,		 const std::string& name = "");

		void Add(const PointLight& pointLight);
		void Add(const DirectionalLight& light);

		void Add(const std::string& path, glm::mat4 transform, const Material& material);
		void Add(const std::string& path, Transform transform);
		void Add(const std::string& path, Transform transform, Material material);

		void SetBackgroundColor(glm::vec3 color);

	private:
		void LoadAssimpNode(const aiNode* node, const aiScene* scene, glm::mat4 transform, const Material& material);

		Scene m_Scene;
	};
}