#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rutile {
    class GLFW {
    public:
        GLFW();
        GLFW(const GLFW& other) = default;
        GLFW(GLFW&& other) noexcept = default;
        GLFW& operator=(const GLFW& other) = default;
        GLFW& operator=(GLFW&& other) noexcept = default;
        ~GLFW();

        void AttachOntoWindow(GLFWwindow* window);
        void DetachFromWindow(GLFWwindow* window);
    };
}