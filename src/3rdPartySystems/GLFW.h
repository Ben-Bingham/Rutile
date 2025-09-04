#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"

namespace Rutile {
    class GLFW {
    public:
        GLFW();
        GLFW(const GLFW& other) = delete;
        GLFW(GLFW&& other) noexcept = default;
        GLFW& operator=(const GLFW& other) = delete;
        GLFW& operator=(GLFW&& other) noexcept = default;
        ~GLFW();

        void InitializeCallbacks(Window& window);
        void InitializeOpenGLDebug();

        void PollEvents();
    };
}