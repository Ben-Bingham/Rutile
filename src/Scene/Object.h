#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Material.h"
#include "Mesh.h"

namespace Rutile {
	struct Object {
		glm::mat4 transform{ 1.0 };

		Mesh mesh;
		std::shared_ptr<Material> material;
	};
}