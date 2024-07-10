#pragma once
#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class CPURayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;
    };
}