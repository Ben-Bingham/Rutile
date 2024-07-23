#include "CameraMovement.h"

#include "events/Events.h"

#include "Settings/App.h"

namespace Rutile {
    void MoveCamera() {
        bool somethingHasChanged = false;
        const float dt = static_cast<float>(App::timingData.frameTime.count());
        const float velocity = App::camera.speed * dt;

        if (glfwGetKey(App::window, GLFW_KEY_W) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position += App::camera.frontVector * velocity;
        }
        if (glfwGetKey(App::window, GLFW_KEY_S) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position -= App::camera.frontVector * velocity;
        }
        if (glfwGetKey(App::window, GLFW_KEY_D) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position += App::camera.rightVector * velocity;
        }
        if (glfwGetKey(App::window, GLFW_KEY_A) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position -= App::camera.rightVector * velocity;
        }
        if (glfwGetKey(App::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position += App::camera.upVector * velocity;
        }
        if (glfwGetKey(App::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            somethingHasChanged = true;
            App::camera.position -= App::camera.upVector * velocity;
        }

        if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            if (App::mouseDown == false) {
                App::lastMousePosition.x = App::mousePosition.x;
                App::lastMousePosition.y = App::mousePosition.y;
            }

            App::mouseDown = true;
        }

        if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            App::mouseDown = false;
        }

        if (App::mouseDown) {
            const float xDelta = (float)App::mousePosition.x - (float)App::lastMousePosition.x;
            const float yDelta = (float)App::lastMousePosition.y - (float)App::mousePosition.y; // reversed since y-coordinates go from bottom to top

            App::camera.yaw += xDelta * App::camera.lookSensitivity;
            App::camera.pitch += yDelta * App::camera.lookSensitivity;

            if (App::camera.pitch > 89.9f) {
                App::camera.pitch = 89.9f;
            }
            else if (App::camera.pitch < -89.9f) {
                App::camera.pitch = -89.9f;
            }

            somethingHasChanged = true;
        }

        if (App::mouseDown || App::updateCameraVectors) {
            App::camera.frontVector.x = cos(glm::radians(App::camera.yaw)) * cos(glm::radians(App::camera.pitch));
            App::camera.frontVector.y = sin(glm::radians(App::camera.pitch));
            App::camera.frontVector.z = sin(glm::radians(App::camera.yaw)) * cos(glm::radians(App::camera.pitch));
            App::camera.frontVector = glm::normalize(App::camera.frontVector);

            App::camera.rightVector = glm::normalize(glm::cross(App::camera.frontVector, App::camera.upVector));

            App::lastMousePosition.x = App::mousePosition.x;
            App::lastMousePosition.y = App::mousePosition.y;

            App::updateCameraVectors = false;
        }

        if (somethingHasChanged) {
            App::eventManager.Notify(new CameraUpdate{ });
        }
    }
}