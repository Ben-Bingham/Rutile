#include "SoftwarePhong.h"

#include <iostream>

#include "Settings/App.h"
#include "Utility/OpenGl/GLDebug.h"

namespace Rutile {
    GLFWwindow* SoftwarePhong::Init() {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        GLFWwindow* window = glfwCreateWindow(App::screenWidth, App::screenHeight, App::name.c_str(), nullptr, nullptr);
        glfwShowWindow(window);

        if (!window) {
            std::cout << "ERROR: Failed to create window." << std::endl;
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK) {
            std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
        }

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        return window;

    }

    void SoftwarePhong::Cleanup(GLFWwindow* window) {

    }

    void SoftwarePhong::Render() {

    }

    void SoftwarePhong::Notify(Event* event) {

    }

    void SoftwarePhong::LoadScene() {

    }
}