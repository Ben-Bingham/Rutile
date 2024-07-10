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

    private:
        unsigned int PixelShader(unsigned int x, unsigned int y);

        unsigned int m_ShaderProgram;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        unsigned int m_ScreenTexture;
    };
}