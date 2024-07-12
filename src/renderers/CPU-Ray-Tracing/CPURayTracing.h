#pragma once
#include <chrono>

#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "utility/ThreadPool.h"

namespace Rutile {
    struct Section {
        size_t startIndex;
        size_t length;

        std::vector<unsigned int> pixels;
    };

    void RenderSection(Section* section);
    unsigned int RenderPixel(unsigned int x, unsigned int y);

    class CPURayTracing : public Renderer {
    public:
        using RayTracingThreadPool = ThreadPool<Section*>;

        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        // Events
        void WindowResizeEvent() override;

        // GUI
        void ProvideTimingStatistics() override;
        void ProvideLocalRendererSettings() override;

    private:
        std::unique_ptr<RayTracingThreadPool> m_ThreadPool;

        int m_SectionCount{ 1 };
        std::vector<Section> m_Sections;

        void CalculateSections();

        // Timing Statistics
        std::chrono::duration<double> m_PixelRenderTime;
        std::chrono::duration<double> m_SectionCombinationTime;

        // Presenting Image
        unsigned int m_ShaderProgram;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        unsigned int m_ScreenTexture;
    };
}