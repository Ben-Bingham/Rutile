#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Window.h"

namespace Rutile {
    class ImGuiInstance {
    public:
        void Init(Window& window);
        void Cleanup();

        void StartNewFrame();
        void FinishFrame();
    };
}