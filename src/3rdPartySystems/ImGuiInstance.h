#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Window.h"

namespace Rutile {
    class ImGuiInstance {
    public:
        ImGuiInstance(Window& window);
        ImGuiInstance(const ImGuiInstance& other) = delete;
        ImGuiInstance(ImGuiInstance&& other) noexcept = default;
        ImGuiInstance& operator=(const ImGuiInstance& other) = delete;
        ImGuiInstance& operator=(ImGuiInstance&& other) noexcept = default;
        ~ImGuiInstance();

        void StartNewFrame();
        void FinishFrame();
    };
}