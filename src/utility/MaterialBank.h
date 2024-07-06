#pragma once

#include "rendering/Material.h"

namespace Rutile {
	using MaterialIndex = size_t;

	class MaterialBank {
	public:
		MaterialBank() = default;

		MaterialIndex Add(const std::string& name, const Solid& solid, const Phong& phong);

		Material* operator[](MaterialIndex i);

		std::string GetName(MaterialIndex i);

	private:
		std::vector<Solid> m_SolidMaterials;
		std::vector<Phong> m_PhongMaterials;

		std::vector<std::string> m_Names;
	};
}