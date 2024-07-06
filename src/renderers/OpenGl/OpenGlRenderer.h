#pragma once

#include <memory>

#include "../Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "utility/Shader.h"

namespace Rutile {
	class OpenGlRenderer : public Renderer {
	public:
        OpenGlRenderer() = default;
        OpenGlRenderer(const OpenGlRenderer& other) = default;
        OpenGlRenderer(OpenGlRenderer&& other) noexcept = default;
        OpenGlRenderer& operator=(const OpenGlRenderer& other) = default;
        OpenGlRenderer& operator=(OpenGlRenderer&& other) noexcept = default;
        ~OpenGlRenderer() override = default;

        // Necessities
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;

        void Render() override;

    private:
        void RenderOmnidirectionalShadowMaps();

        void RenderScene();

	public:
        // Events
        void ProjectionMatrixUpdate() override;

        void SignalNewScene() override;

        void ProvideLightVisualization(LightIndex lightIndex) override;
        void ProvideDirectionalLightVisualization() override;

	private:
        glm::mat4 m_Projection { 1.0f };

        // Shaders
        std::unique_ptr<Shader> m_SolidShader;
        std::unique_ptr<Shader> m_PhongShader;

        std::unique_ptr<Shader> m_DirectionalShadowMappingShader;
        std::unique_ptr<Shader> m_OmnidirectionalShadowMappingShader;
        std::unique_ptr<Shader> m_CubeMapVisualizationShader;

        // Omnidirectional Shadow Maps
        std::vector<float> m_OmnidirectionalShadowMapVisualizationHorizontalOffsets{ 0.0f };
        std::vector<float> m_OmnidirectionalShadowMapVisualizationVerticalOffsets{ 0.0f };

        unsigned int m_OmnidirectionalShadowMapFBO;

        int m_OmnidirectionalShadowMapWidth{ 1024 };
        int m_OmnidirectionalShadowMapHeight{ 1024 };

        std::vector<unsigned int> m_PointLightCubeMaps;

        unsigned int m_CubeMapVisualizationFBO{ 0 };
        unsigned int m_CubeMapVisualizationRBO{ 0 };

        std::vector<unsigned int> m_CubeMapVisualizationTextures;

        void VisualizeCubeMap(LightIndex lightIndex);

        // Objects
        size_t m_ObjectCount;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;

        // Shadow Map
        //unsigned int m_DepthMapFBO;

        //unsigned int m_ShadowMapTexture;

        //int m_DirectionalShadowMapWidth{ 1024 };
        //int m_DirectionalShadowMapHeight{ 1024 };

        //glm::vec3 m_DirectionalLightPosition{ 3.0f, 3.0f, 3.0f};

        //glm::mat4 m_LightSpaceMatrix{ 1.0f };

        //float m_DirectionalLightLeft{ -1.0f };
        //float m_DirectionalLightRight{ 5.0f };
        //float m_DirectionalLightBottom{ -4.0f };
        //float m_DirectionalLightTop{ 3.0f };

        //float m_DirectionalLightNear{ 0.001f };
        //float m_DirectionalLightFar{ 11.0f };
	};
}