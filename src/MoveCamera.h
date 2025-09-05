#pragma once

#include <glm/glm.hpp>

#include "Scene/Camera.h"
#include "3rdPartySystems/Window.h"

namespace Rutile {
	void MoveCamera(Camera& camera, Window& window, float dt, const glm::ivec2& mousePositionWRTViewport, const glm::ivec2& viewportSize);
}