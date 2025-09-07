#pragma once

#include <vector>
#include <memory>

#include "Light.h"
#include "Object.h"

namespace Rutile {
	struct Scene {
		std::vector<Object> objects;
		std::vector<std::shared_ptr<Light>> lights; // TODO remove pointer
		// Needs to be a pointer so that it can be null, not all scenes will have a directional light
		// Needs to be shared so that the scene can be passed to renderers
		std::shared_ptr<DirectionalLight> directionalLight;
	};
}