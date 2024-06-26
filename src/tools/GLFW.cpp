#include "GLFW.h"

#include "App.h"

#include <iostream>

#include "renderers/Renderer.h"

void glfwErrorCallback(int error, const char* description) {
    std::cout << "ERROR: GLFW has thrown an error: " << std::endl;
    std::cout << description << std::endl;
}

namespace Rutile {
    void frameBufferSizeCallback(GLFWwindow* window, int w, int h) {
        App::screenWidth = w;
        App::screenHeight = h;

        if (App::renderer) {
            App::renderer->WindowResize();
        }
    }

    void mouseMoveCallback(GLFWwindow* window, double x, double y) {
        App::mousePosition.x = static_cast<int>(x);
        App::mousePosition.y = static_cast<int>(y);
    }

    void GLFW::Init() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
        }
    }

    void GLFW::Cleanup() {
        glfwTerminate();
    }

    void GLFW::AttachOntoWindow(GLFWwindow* window) {
        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
        glfwSetCursorPosCallback(window, mouseMoveCallback);
    }

    void GLFW::DetachFromWindow(GLFWwindow* window) {
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetFramebufferSizeCallback(window, nullptr);
    }
}