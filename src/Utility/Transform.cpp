#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Rutile {
    void Transform::CalculateMatrix() {
        const glm::mat4 translationMatrix = glm::translate(glm::mat4{ 1.0f }, position);

        const glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        matrix = translationMatrix * rotationMatrix * scaleMatrix;
    }
}