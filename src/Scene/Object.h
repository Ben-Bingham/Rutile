#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Material.h"

namespace Rutile {
	struct Object {
		glm::mat4 transform{ 1.0 };
		std::shared_ptr<Material> material;
	};
}