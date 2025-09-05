#include "MoveCamera.h"

namespace Rutile {
    void MoveCamera(Camera& camera, Window& window, float dt, const glm::ivec2& mousePositionWRTViewport, const glm::ivec2& viewportSize) {
        static bool mouseDown{ false };
        static bool hasMoved{ false };
        static glm::ivec2 lastMousePosition{ };

        if (!hasMoved) {
            lastMousePosition = mousePositionWRTViewport;
            hasMoved = true;
        }

        bool positionChange{ false };
        bool directionChange{ false };
        const float velocity = camera.speed * dt;

        if (glfwGetKey(window.Get(), GLFW_KEY_W) == GLFW_PRESS) {
            positionChange = true;
            camera.position += camera.frontVector * velocity;
        }
        if (glfwGetKey(window.Get(), GLFW_KEY_S) == GLFW_PRESS) {
            positionChange = true;
            camera.position -= camera.frontVector * velocity;
        }
        if (glfwGetKey(window.Get(), GLFW_KEY_D) == GLFW_PRESS) {
            positionChange = true;
            camera.position += camera.rightVector * velocity;
        }
        if (glfwGetKey(window.Get(), GLFW_KEY_A) == GLFW_PRESS) {
            positionChange = true;
            camera.position -= camera.rightVector * velocity;
        }
        if (glfwGetKey(window.Get(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            positionChange = true;
            camera.position -= camera.upVector * velocity;
        }
        if (glfwGetKey(window.Get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            positionChange = true;
            camera.position += camera.upVector * velocity;
        }

        if (glfwGetMouseButton(window.Get(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            if (mouseDown == false) {
                lastMousePosition.x = mousePositionWRTViewport.x;
                lastMousePosition.y = mousePositionWRTViewport.y;
            }

            mouseDown = true;
        }

        if (glfwGetMouseButton(window.Get(), GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            mouseDown = false;
        }

        bool mouseOverViewport = mousePositionWRTViewport.x >= 0 &&
            mousePositionWRTViewport.y >= 0 &&
            mousePositionWRTViewport.x < viewportSize.x &&
            mousePositionWRTViewport.y < viewportSize.y;

        if (!mouseOverViewport) {
            hasMoved = false;
        }

        if (mouseDown && mouseOverViewport) {
            const float xDelta = (float)mousePositionWRTViewport.x - (float)lastMousePosition.x;
            const float yDelta = (float)mousePositionWRTViewport.y - (float)lastMousePosition.y;

            camera.yaw += xDelta * camera.lookSensitivity;
            camera.pitch += yDelta * camera.lookSensitivity;

            if (camera.pitch > 89.9f) {
                camera.pitch = 89.9f;
            }
            else if (camera.pitch < -89.9f) {
                camera.pitch = -89.9f;
            }

            directionChange = true;
        }

        if (mouseDown
            //|| updateCameraVectors  // TODO add back if you can adjust camera from IMGUI
            && mouseOverViewport) {
            camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            camera.frontVector.y = sin(glm::radians(camera.pitch));
            camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            camera.frontVector = glm::normalize(camera.frontVector);

            camera.rightVector = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

            lastMousePosition.x = mousePositionWRTViewport.x;
            lastMousePosition.y = mousePositionWRTViewport.y;

            //App::updateCameraVectors = false;
        }
    }
}