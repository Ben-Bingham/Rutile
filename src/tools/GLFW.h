#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rutile {
    class GLFW {
    public:
        GLFW() = default;

        void Init();
        void Cleanup();

        void AttachOntoWindow(GLFWwindow* window);
        void DetachFromWindow(GLFWwindow* window);
    };
}