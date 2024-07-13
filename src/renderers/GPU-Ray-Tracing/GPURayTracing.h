#pragma once
#include <memory>

#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "renderers/OpenGl/utility/Shader.h"

namespace Rutile {
    class GPURayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        // Events
        void WindowResizeEvent() override;

    private:
        std::unique_ptr<Shader> m_RayTracingShader;

        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        unsigned int m_EBO{ 0 };
    };
}