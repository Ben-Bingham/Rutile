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

    unsigned int RenderPixel(glm::u32vec2 pixelCoordinate);
    void RenderSection(Section* section);

    struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    glm::vec3 FireRayIntoScene(const Ray& ray);

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

        int m_SectionCount{ 16 };
        std::vector<Section> m_Sections;

        void CalculateSections();

        // Timing Statistics
        std::chrono::duration<double> m_PixelRenderTime{ };
        std::chrono::duration<double> m_SectionCombinationTime{ };

        // Presenting Image
        unsigned int m_ShaderProgram{ 0 };

        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        unsigned int m_EBO{ 0 };

        unsigned int m_ScreenTexture{ 0 };
    };
}