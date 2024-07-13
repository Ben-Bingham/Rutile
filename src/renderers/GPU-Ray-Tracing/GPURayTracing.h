#pragma once
#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class GPURayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        // Events
        void WindowResizeEvent() override;

    private:
        unsigned int m_ShaderProgram{ 0 };

        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        unsigned int m_EBO{ 0 };

        unsigned int m_ScreenTexture{ 0 };
    };
}