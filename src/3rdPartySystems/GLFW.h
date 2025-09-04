#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class GLFW {
    public:
        void Init();
        void Cleanup();

        void InitializeOpenGLDebug();

        void AttachOntoWindow(GLFWwindow* window);
        void DetachFromWindow(GLFWwindow* window);
    };
}