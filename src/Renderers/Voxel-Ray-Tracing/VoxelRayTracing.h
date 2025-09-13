#pragma once
#include <chrono>
#include <memory>

#include "Renderer.h"

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/SSBO.h"

#include "Renderers/GPU-Ray-Tracing/Obj.h"

namespace Rutile {
    using MaterialIndex = uint8_t;

    class VoxelRayTracing : public Renderer {
    public:
        VoxelRayTracing();
        VoxelRayTracing(const VoxelRayTracing& other) = delete;
        VoxelRayTracing(VoxelRayTracing&& other) noexcept = default;
        VoxelRayTracing& operator=(const VoxelRayTracing& other) = delete;
        VoxelRayTracing& operator=(VoxelRayTracing&& other) noexcept = default;
        ~VoxelRayTracing() override;

        void Render(RenderTarget& target, const Camera& camera) override;

        void ProvideGeneralGUI() override;

        void SetScene(Scene scene) override;

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
        Camera m_Camera{ };

        std::vector<Obj> m_Objects{ };

        glm::vec3 m_BackgroundColor{ };

        int m_MaxBounces{ 5 };

        bool m_CreateAccumulationBuffer{ true };
        void CreateAccumulationBuffer(glm::ivec2 screenSize);

        struct LocalMaterial {
            int type;
            float fuzz;
            float indexOfRefraction;
            alignas(16) glm::vec4 color;
        };

        bool m_ResetAcumulatedPixelData{ true };
        void ResetAccumulatedPixelData(glm::ivec2 screenSize);
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