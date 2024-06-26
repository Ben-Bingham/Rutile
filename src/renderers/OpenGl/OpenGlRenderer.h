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

        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;

        void Render() override;

        void SetScene(const Scene& scene) override;

        void WindowResize() override;

        //void SetPacket(size_t index, Packet packet) override;
        //void SetLight(size_t index, LightType type, Light* light) override;

        //void AddPacket(Packet packet) override;
        //void AddLight(LightType type, Light* light) override;

        void UpdatePacketTransform(size_t index) override;

        void UpdateFieldOfView() override;
        void UpdateNearPlane() override;
        void UpdateFarPlane() override;

        void UpdateDirectionalShadowMap() override;

        void UpdateOmnidirectionalShadowMap() override;

        void UpdateShadowMapMode() override;

        void ProvideLightVisualization(size_t i) override;
        void ProvideDirectionalLightVisualization() override;

    private:
        void UpdateProjectionMatrix();

        glm::mat4 m_Projection { 1.0f };

        // Shaders
        std::unique_ptr<Shader> m_SolidShader;
        std::unique_ptr<Shader> m_PhongShader;
        std::unique_ptr<Shader> m_DirectionalShadowMappingShader;
        std::unique_ptr<Shader> m_OmnidirectionalShadowMappingShader;
        std::unique_ptr<Shader> m_CubeMapVisualizationShader;

        // Lights
        std::vector<PointLight*> m_PointLights;
        DirectionalLight* m_DirectionalLight;
        std::vector<SpotLight*> m_SpotLights;

        // These vectors are the same size as there counterparts above that actually store the lights, however these
        // vectors store the global index (as per the Scene) for each light
        std::vector<size_t> m_PointLightIndices;
        std::vector<size_t> m_SpotLightIndices;

        // Omnidirectional Shadow Maps
        float m_HorizontalModifier{ 0.0f };
        float m_VerticalModifier{ 0.0f };

        unsigned int m_OmnidirectionalShadowMapFBO;

        int m_OmnidirectionalShadowMapWidth{ 1024 };
        int m_OmnidirectionalShadowMapHeight{ 1024 };

        std::vector<unsigned int> m_PointLightCubeMaps;

        unsigned int m_CubeMapVisualizationFBO{ 0 };
        unsigned int m_CubeMapVisualizationTexture{ 0 };
        unsigned int m_CubeMapVisualizationRBO{ 0 };
        void VisualizeCubeMap(unsigned int cubemap);

        // Packets
        size_t m_PacketCount;

        std::vector<bool> m_ValidPackets;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;

        std::vector<size_t> m_IndexCounts;
        std::vector<Transform*> m_Transforms;

        std::vector<MaterialType> m_MaterialTypes;
        std::vector<Material*> m_Materials;

        // Shadow Map
        unsigned int m_DepthMapFBO;

        unsigned int m_ShadowMapTexture;

        int m_DirectionalShadowMapWidth{ 1024 };
        int m_DirectionalShadowMapHeight{ 1024 };

        glm::vec3 m_DirectionalLightPosition{ 3.0f, 3.0f, 3.0f};

        glm::mat4 m_LightSpaceMatrix{ 1.0f };

        float m_DirectionalLightLeft{ -1.0f };
        float m_DirectionalLightRight{ 5.0f };
        float m_DirectionalLightBottom{ -4.0f };
        float m_DirectionalLightTop{ 3.0f };

        float m_DirectionalLightNear{ 0.001f };
        float m_DirectionalLightFar{ 11.0f };
	};
}