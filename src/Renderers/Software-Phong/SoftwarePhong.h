#pragma once

#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class SoftwarePhong : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        void Notify(Event* event) override;

        void LoadScene() override;

    private:
        // Presenting Image
        unsigned int m_ShaderProgram{ 0 };

        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        unsigned int m_EBO{ 0 };

        unsigned int m_ScreenTexture{ 0 };
    };
}