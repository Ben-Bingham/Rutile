#include "GLFW.h"

#include "Settings/App.h"

#include <iostream>

//#include "renderers/Renderer.h"

#include "Utility/OpenGl/GLDebug.h"

void glfwErrorCallback(int error, const char* description) {
    std::cout << "ERROR: GLFW has thrown an error: " << std::endl;
    std::cout << description << std::endl;
}

namespace Rutile {
    void frameBufferSizeCallback(GLFWwindow* window, int w, int h) {
        App::screenWidth = w;
        App::screenHeight = h;

        //if (App::renderer) {
            //App::eventManager.Notify(new WindowResize{ });
        //}
    }

    void mouseMoveCallback(GLFWwindow* window, double x, double y) {
        App::mousePosition.x = static_cast<int>(x);
        App::mousePosition.y = static_cast<int>(y);
    }

    GLFW::GLFW() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
        }
    }

    GLFW::~GLFW() {
        glfwTerminate();

    }

    void GLFW::InitializeOpenGLDebug() {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
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