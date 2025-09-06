#pragma once
#include "Scene/Material.h"

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

		static Material Construct(const glm::vec3& color);
		static Material Construct(Color color);
	};
}