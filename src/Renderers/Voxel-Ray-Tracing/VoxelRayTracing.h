#pragma once
#include <chrono>
#include <memory>

#include "renderers/Renderer.h"

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/SSBO.h"
#include "Utility/Voxels/Chunk.h"

namespace Rutile {
    class VoxelRayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;

        void Notify(Event* event) override;
        void Render() override;
        void LoadScene() override;

        void ProvideLocalRendererSettings() override;

        constexpr static size_t xSize = 8;
        constexpr static size_t ySize = 8;
        constexpr static size_t zSize = 8;
        constexpr static size_t materialCount = 16;

        //using Chunk = Chunk<xSize, ySize, zSize, materialCount>;

        void CreateOctree();

        struct Voxel {
            glm::vec3 minBound;
            glm::vec3 maxBound;

            int k0{ -1 };
            int k1{ -1 };
            int k2{ -1 };
            int k3{ -1 };
            int k4{ -1 };
            int k5{ -1 };
            int k6{ -1 };
            int k7{ -1 };

            int hasKids{ false };
            int shouldDraw{ false };
        };

        std::vector<Voxel> voxels;

    private:
        bool m_OctTreeX{ false };
        bool m_OctTreeY{ false };
        bool m_OctTreeZ{ false };

        bool m_OctTreeNoKids{ false };

        bool m_OctTreeXL2{ false };
        bool m_OctTreeYL2{ false };
        bool m_OctTreeZL2{ false };

        float m_StartingWidth{ 1.0f };

        bool m_Child0{ false };
        bool m_Child1{ false };
        bool m_Child2{ false };
        bool m_Child3{ false };
        bool m_Child4{ false };
        bool m_Child5{ false };
        bool m_Child6{ false };
        bool m_Child7{ false };

        int hardCodedOctree;

        void ResetAccumulatedPixelData();

        std::chrono::time_point<std::chrono::steady_clock> m_RendererLoadTime;

        std::unique_ptr<Shader> m_VoxelRayTracingShader;
        std::unique_ptr<Shader> m_RenderingShader;

        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;

        size_t m_FrameCount{ 0 };

        unsigned int m_AccumulationFrameBuffer{ 0 };
        unsigned int m_AccumulationTexture{ 0 };
        unsigned int m_AccumulationRBO{ 0 };

        Chunk m_Chunk;

        std::unique_ptr<SSBO<unsigned char>> m_BlockData;
        std::unique_ptr<SSBO<Voxel>> m_VoxelSSBO;
    };
}