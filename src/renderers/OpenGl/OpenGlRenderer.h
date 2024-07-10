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

        void RenderCascadingShadowMaps();

        void RenderScene();

        std::vector<glm::vec4> GetFrustumCornersInWorldSpace(const glm::mat4& frustum);

	public:
        // Events
        void ProjectionMatrixUpdate() override;

        void SignalNewScene() override;

        void ProvideLightVisualization(LightIndex lightIndex) override;

        void ProvideCSMVisualization() override;

    private:
        glm::mat4 m_Projection { 1.0f };

        // Shaders
        std::unique_ptr<Shader> m_SolidShader;
        std::unique_ptr<Shader> m_PhongShader;

        std::unique_ptr<Shader> m_OmnidirectionalShadowMappingShader;
        std::unique_ptr<Shader> m_CubeMapVisualizationShader;

        std::unique_ptr<Shader> m_CascadingShadowMapShader;
        std::unique_ptr<Shader> m_CascadingShadowMapVisualizationShader;

        // Omnidirectional Shadow maps
        std::vector<float> m_OmnidirectionalShadowMapVisualizationHorizontalOffsets{ 0.0f };
        std::vector<float> m_OmnidirectionalShadowMapVisualizationVerticalOffsets{ 0.0f };

        unsigned int m_OmnidirectionalShadowMapFBO;

        int m_OmnidirectionalShadowMapWidth{ 1024 };
        int m_OmnidirectionalShadowMapHeight{ 1024 };

        std::vector<unsigned int> m_PointLightCubeMaps;

        unsigned int m_CubeMapVisualizationFBO{ 0 };
        unsigned int m_CubeMapVisualizationRBO{ 0 };

        std::vector<unsigned int> m_CubeMapVisualizationTextures;

        int m_CubeMapVisualizationWidth{ 1024 };
        int m_CubeMapVisualizationHeight{ 512 };

        void VisualizeCubeMap(LightIndex lightIndex);

        // Cascading Shadow maps
        unsigned int m_CascadingShadowMapFBO;
        unsigned int m_CascadingShadowMapTexture;

        int m_CascadingShadowMapWidth { 1024 };
        int m_CascadingShadowMapHeight{ 1024 };

        int m_CascadeCount{ 5 };
        int m_MaxCascadeCount{ 10 };

        unsigned int m_ShadowCascadesVisualizationFBO{ 0 };
        unsigned int m_ShadowCascadesVisualizationRBO{ 0 };
        unsigned int m_ShadowCascadesVisualizationTexture{ 0 };

        int m_DisplayedCascadeLayer{ 0 };

        int m_CascadeVisualizationWidth{ 1024 };
        int m_CascadeVisualizationHeight{ 1024 };

        float m_ZMinMultiplier{ 1.5f };
        float m_ZMaxMultiplier{ 1.5f };

        std::vector<glm::mat4> m_CascadeCameraProjections;
        std::vector<std::pair<glm::vec3, glm::vec3>> m_CascadeLightBoxes;
        std::vector<float> m_CascadingFrustumPlanes;
        std::vector<glm::mat4> m_LightSpaceMatrices;

        void VisualizeCascadeShadowMap(int layer);

        void VisualizeShadowCascades();
        void VisualizeCascadeLights();

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