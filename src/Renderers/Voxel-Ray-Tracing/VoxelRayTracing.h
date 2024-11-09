#pragma once
#include <chrono>
#include <memory>

#include "renderers/Renderer.h"

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/SSBO.h"

namespace Rutile {
    class VoxelRayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;

        void Notify(Event* event) override;
        void Render() override;
        void LoadScene() override;

        void ProvideLocalRendererSettings() override;

        void ProjectionMatrixUpdate() override;

        void CreateOctree();

        struct Voxel {
            glm::vec3 minBound;
            glm::vec3 maxBound;

            int k0{ -1 };

            int hasKids{ false };
            int shouldDraw{ false };

            int childMask{ 0 }; // first 8 bits dictate which children the voxel has
        };

        std::vector<Voxel> voxels;

    private:
        struct LocalMaterial {
            int type;
            float fuzz;
            float indexOfRefraction;
            alignas(16) glm::vec4 color;
        };

        void ResetAccumulatedPixelData();
        void CreateAndUploadMaterialBuffer();

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

        std::unique_ptr<SSBO<Voxel>> m_VoxelSSBO;

        std::unique_ptr<SSBO<LocalMaterial>> m_MaterialBank;
    };
}