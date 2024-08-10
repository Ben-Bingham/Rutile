#pragma once
#include <chrono>
#include <memory>

#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/SSBO.h"
#include "Utility/RayTracing/BoundingVolumeHierarchy/BVHIndex.h"

namespace Rutile {
    class GPURayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;
        void Render() override;

        void LoadScene() override;

        void SignalRayTracingSettingsChange() override;

        void Notify(Event* event) override;

        // ImGui
        void ProvideLocalRendererSettings() override;

    private:
        void ResetAccumulatedPixelData();
        void UploadObjectAndMaterialBuffers();

        //void CreateAndUploadMaterialBuffer();
        //void CreateAndUploadObjectBuffer();
        //void CreateAndUploadMeshBuffer();
        //void CreateAndUploadTLASBuffer();
        //void CreateAndUploadBLASBuffer();

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