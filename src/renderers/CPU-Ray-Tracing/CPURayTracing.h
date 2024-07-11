#pragma once
#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    class CPURayTracing : public Renderer {
    public:
        struct Section {
            size_t startIndex;
            size_t length;

            std::vector<unsigned int> pixels;
        };

        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        // Events
        void WindowResizeEvent() override;

    private:
        unsigned int RenderPixel(unsigned int x, unsigned int y);

        std::vector<Section> m_Sections;

        void CalculateSections();
        void RenderSection(Section& section);

        std::vector<unsigned int> CombineSections();

        unsigned int m_XSectionCount{ 4 };
        unsigned int m_YSectionCount{ 4 };

        // Presenting Image
        unsigned int m_ShaderProgram;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        unsigned int m_ScreenTexture;
    };
}