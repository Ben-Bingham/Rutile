#pragma once
#include <chrono>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Utility/ThreadPool.h"

#include "Renderer.h"

#include "Scene/Scene.h"

namespace Rutile {
    struct Section {
        size_t startIndex;
        size_t length;

        std::vector<glm::vec4> pixels;

        glm::ivec2 screenSize;
        Camera camera;
        Scene scene; // TODO this is expensive probably
    };

    glm::vec4 RenderPixel(glm::u32vec2 pixelCoordinate, const Camera& camera, glm::ivec2 screenSize, Scene scene);
    void RenderSection(Section* section);

    struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    glm::vec3 FireRayIntoScene(Ray ray, Scene scene);

    class CPURayTracing : public Renderer {
    public:
        using RayTracingThreadPool = ThreadPool<Section*>;

        CPURayTracing();
        CPURayTracing(const CPURayTracing& other) = delete;
        CPURayTracing(CPURayTracing&& other) noexcept = default;
        CPURayTracing& operator=(const CPURayTracing& other) = delete;
        CPURayTracing& operator=(CPURayTracing&& other) noexcept = default;
        ~CPURayTracing() override;

        void Render(RenderTarget& target, const Camera& camera) override;

        void SetScene(Scene scene) override;

        // GUI
        void ProvideGeneralGUI() override;

        void UpdateObjectTransform(size_t i, const glm::mat4& newTransform) override;
        void UpdateObjectMaterial(size_t i, const std::shared_ptr<Material> newMaterial) override;

        static inline glm::vec3 backgroundColour{ 0.5f }; // TODO make this not static

    private:
        Scene m_Scene;

        void ResetAccumulatedPixelData(glm::ivec2 screenSize);

        bool m_ResetAccumulatedPixelData{ true };
        std::vector<glm::vec4> m_AccumulatedPixelData;
        int m_FrameCount{ 0 };

        std::unique_ptr<RayTracingThreadPool> m_ThreadPool;

        int m_SectionCount{ 16 };
        bool m_SectionCountChange{ true };
        std::vector<Section> m_Sections;

        void CalculateSections(glm::ivec2 screenSize);

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