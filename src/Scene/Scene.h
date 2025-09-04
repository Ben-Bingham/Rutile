#pragma once

#include <vector>
#include <memory>

#include "Light.h"
#include "Object.h"

namespace Rutile {
	struct Scene {
		std::vector<Object> objects;
		std::vector<std::shared_ptr<Light>> lights;
	};
}