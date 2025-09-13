#pragma once
#include <chrono>
#include <memory>

#include "Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/SSBO.h"
#include "Utility/RayTracing/BoundingVolumeHierarchy/BVHIndex.h"

#include "Scene/Scene.h"

#include "Obj.h"

namespace Rutile {
    class GPURayTracing : public Renderer {
    public:
        GPURayTracing();
        GPURayTracing(const GPURayTracing& other) = delete;
        GPURayTracing(GPURayTracing&& other) noexcept = default;
        GPURayTracing& operator=(const GPURayTracing& other) = delete;
        GPURayTracing& operator=(GPURayTracing&& other) noexcept = default;
        ~GPURayTracing() override;

        void Render(RenderTarget& target, const Camera& camera) override;

        void ProvideGeneralGUI() override;

        void SetScene(Scene scene) override;

    private:
        std::vector<Obj> m_Objects{ };


        glm::vec3 m_BackgroundColour{ 0.0f };

        int m_MaxBounces{ 5 };

        //Scene m_Scene;

        glm::ivec2 m_ScreenSize;
        Camera m_Camera;

        bool m_CreatedAccumulationBuffer{ false };
        void CreateAccumulationBuffer(glm::ivec2 screenSize);

        bool m_ResetAccumulatedPixelData{ true };
        void ResetAccumulatedPixelData(glm::ivec2 screenSize);

        void CreateAndUploadMaterialBuffer();
        void CreateAndUploadBVHAndMeshAndObjectBuffers();

        int m_FrameCount{ 0 };

        unsigned int m_AccumulationFrameBuffer{ 0 };
        unsigned int m_AccumulationTexture{ 0 };
        unsigned int m_AccumulationRBO{ 0 };

        std::chrono::time_point<std::chrono::steady_clock> m_RendererLoadTime;

        std::unique_ptr<Shader> m_RayTracingShader;
        std::unique_ptr<Shader> m_RenderingShader;

        unsigned int m_VAO{ 0 };
        unsigned int m_VBO{ 0 };
        unsigned int m_EBO{ 0 };

        struct LocalMaterial {
            int type;
            float fuzz;
            float indexOfRefraction;
            alignas(16) glm::vec4 color;
        };

        struct LocalObject {
            glm::mat4 model;
            glm::mat4 invModel;

            glm::mat4 transposeInverseModel;
            glm::mat4 transposeInverseInverseModel;

            int materialIndex;
            int geometryType;
            int BVHStartIndex;
            int meshSize;
        };

        struct LocalTLASNode {
            glm::vec3 min;
            glm::vec3 max;

            BVHIndex node1;
            BVHIndex node2;
        };

        struct LocalBLASNode {
            glm::vec3 min;
            glm::vec3 max;

            BVHIndex node1Offset;
            int triangleCount;
        };

        std::unique_ptr<SSBO<LocalMaterial>> m_MaterialBank;
        std::unique_ptr<SSBO<LocalObject>> m_ObjectBank;
        std::unique_ptr<SSBO<float>> m_MeshBank;
        std::unique_ptr<SSBO<LocalTLASNode>> m_TLASBank;
        std::unique_ptr<SSBO<LocalBLASNode>> m_BLASBank;
    };
}