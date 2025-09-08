#pragma once

#include <memory>

#include "Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Utility/OpenGl/Shader.h"
#include "Utility/OpenGl/Framebuffer.h"
#include "Utility/OpenGl/Texture.h"
#include "Utility/OpenGl/Renderbuffer.h"

namespace Rutile {
	class OpenGlPhongShading : public Renderer {
	public:
        OpenGlPhongShading();
        OpenGlPhongShading(const OpenGlPhongShading& other) = delete;
        OpenGlPhongShading(OpenGlPhongShading&& other) noexcept = default;
        OpenGlPhongShading& operator=(const OpenGlPhongShading& other) = delete;
        OpenGlPhongShading& operator=(OpenGlPhongShading&& other) noexcept = default;
        ~OpenGlPhongShading() override;

        void Render(RenderTarget& target, const Camera& camera) override;

        void ProvideGeneralGUI() override;

        void ProvideDirectionalLightGUI() override;
        void ProvidePointLightGUI(size_t i) override;

        void SetScene(Scene scene) override;

        void UpdateObjectTransform(size_t i, const glm::mat4& newTransform) override;
        void UpdateObjectMaterial(size_t i, const std::shared_ptr<Material> newMaterial) override;

        void UpdateDirectionalLight(const std::shared_ptr<DirectionalLight> newLight) override;
        void UpdatePointLight(size_t i, const PointLight& newLight) override;

    private:
        glm::mat4 m_Projection{ 1.0f };

        void RenderOmnidirectionalShadowMaps(size_t i);

        void RenderCascadingShadowMaps();

        void RenderScene(RenderTarget& target, const Camera& camera);

        std::vector<glm::vec4> GetFrustumCornersInWorldSpace(const glm::mat4& frustum);

	public:
        // Events
        //void ProjectionMatrixUpdate() override;


        //void ProvideLightVisualization(size_t lightIndex) override; // TODO

        //void ProvideCSMVisualization() override;

    private:
        // Shaders
        std::unique_ptr<Shader> m_PhongShader;

        // Objects
        struct Phong {
            glm::vec3 diffuse{ };
            glm::vec3 ambient{ };
            glm::vec3 specular{ };
            float shininess{ };
        };

        size_t m_ObjectCount;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;
        std::vector<int> m_IndexCounts;
        std::vector<Phong> m_Materials;
        std::vector<glm::mat4> m_Transforms;

        // Point Lights
        struct ShadowMapPointLight : public PointLight {
            ShadowMapPointLight(const PointLight& light);

            glm::ivec2 shadowMapSize{ 1024, 1024 };

            float nearPlane{ 1.0f };
            float farPlane{ 25.0f };

            std::unique_ptr<Cubemap> cubemap{ };

            static constexpr glm::ivec2 cubeMapVisualizationSize{ 1024, 512 };
            std::unique_ptr<Texture2D> cubeMapVisualizationTexture{ };

            glm::vec2 cubeMapVisualizationOffsets{ 0.0f };
        };

        ShadowMapPointLight InitializePointLight(const PointLight& light);

        std::vector<ShadowMapPointLight> m_PointLights{ };

        std::unique_ptr<Shader> m_OmnidirectionalShadowMappingShader;
        std::unique_ptr<Framebuffer> m_OmnidirectionalShadowMapsFramebuffer;

        struct PointLightShadowSettings {
            bool enable{ true };

            float bias{ 0.05f };

            enum class PCFModes {
                NONE,
                FIXED_SAMPLE_COUNT,
                FIXED_SAMPLE_DIRECTIONS
            } pcfMode{ PCFModes::FIXED_SAMPLE_COUNT };

            int sampleCount{ 4 };

            enum class DiskRadiusModes {
                STATIC,
                DYNAMIC
            } diskRadiusMode{ DiskRadiusModes::STATIC };

            float radius{ 0.05f };

        } m_PointLightShadowSettings;

        // Directional Light
        std::shared_ptr<DirectionalLight> m_DirectionalLight{ };
        
        // Cubemap visualization
        std::unique_ptr<Shader> m_CubeMapVisualizationShader;

        void CubeMapToTexture2D(Cubemap& cubemap, Texture2D& texture, glm::ivec2 textureSize, glm::vec2 offset = glm::vec2{ 0.0f }); // TODO move to its own file

        Framebuffer m_CubeMapVisualizationFramebuffer{ };
        Renderbuffer m_CubeMapVisualizationRenderbuffer{ ShadowMapPointLight::cubeMapVisualizationSize };





        // OLD stuff // TODO


        std::unique_ptr<Shader> m_CascadingShadowMapShader;
        std::unique_ptr<Shader> m_CascadingShadowMapVisualizationShader;


        //void VisualizeCubeMap(LightIndex lightIndex);

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

        //void VisualizeCascadeShadowMap(int layer);

        //void VisualizeShadowCascades();
        //void VisualizeCascadeLights();



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