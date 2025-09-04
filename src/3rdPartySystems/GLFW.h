#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class GLFW {
    public:
        GLFW();
        GLFW(const GLFW& other) = delete;
        GLFW(GLFW&& other) noexcept = default;
        GLFW& operator=(const GLFW& other) = delete;
        GLFW& operator=(GLFW&& other) noexcept = default;
        ~GLFW();

        void InitializeOpenGLDebug();

        void AttachOntoWindow(GLFWwindow* window);
        void DetachFromWindow(GLFWwindow* window);
    };
}