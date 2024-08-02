#pragma once
#include <chrono>
#include <memory>

#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "renderers/OpenGl/Utility/Shader.h"

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

        unsigned int m_MaterialBankSSBO{ 0 };
        unsigned int m_ObjectSSBO{ 0 };
        unsigned int m_MeshSSBO{ 0 };
        unsigned int m_TLASSSBO{ 0 };
        unsigned int m_BLASSSBO{ 0 };
    };
}