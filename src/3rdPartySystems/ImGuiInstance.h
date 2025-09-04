#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rutile {
    class ImGuiInstance {
    public:
        void Init(GLFWwindow* window);
        void Cleanup();

        void StartNewFrame();
        void FinishFrame();
    };
}