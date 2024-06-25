#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>

namespace Rutile {
    glm::mat4 Camera::View() const {
        return glm::lookAt(position, position + frontVector, upVector);
    }
}