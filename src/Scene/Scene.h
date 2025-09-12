#pragma once

#include <vector>
#include <memory>

#include "Light.h"
#include "Object.h"

namespace Rutile {
	struct Scene {
		std::vector<Object> objects;
		std::vector<PointLight> pointLights;

		// Needs to be a pointer so that it can be null, not all scenes will have a directional light
		// Needs to be shared so that the scene can be passed to renderers
		std::shared_ptr<DirectionalLight> directionalLight;

		glm::vec3 backgroundColor{ 0.5f, 0.5f, 0.5f };
	};
}