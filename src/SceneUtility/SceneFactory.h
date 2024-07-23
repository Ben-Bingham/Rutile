#pragma once
#include "Scenes/Scene.h"

#include "utility/GeometryFactory.h"
#include "utility/MaterialFactory.h"
#include "utility/RenderingBanks.h"

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

	private:
		Scene m_CurrentScene;

		int m_ObjectNamingIndex{ 0 };
	};
}