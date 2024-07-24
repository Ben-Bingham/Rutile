#pragma once
#include "RenderingAPI/Material.h"

namespace Rutile {
	class MaterialFactory {
	public:
		enum class Color {
		    RED,
			GREEN,
			BLUE,
			YELLOW,
			PINK,
			CYAN,
			WHITE,
			BLACK,
			GRAY
		};

		static Material Construct(const Material::Solid& solid, const std::string& name = "");
		static Material Construct(const Material::Phong& phong, const std::string& name = "");

		static Material Construct(const glm::vec3& color, const std::string& name = "");
		static Material Construct(Color color, const std::string& name = "");

	private:
		inline static int m_MaterialNamingIndex{ 0 };
	};
}