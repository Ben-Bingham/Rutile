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

        void ProvideLightVisualization(size_t i) override;

    private:
        void UpdateProjectionMatrix();

        glm::mat4 m_Projection { 1.0f };

        // Shaders
        std::unique_ptr<Shader> m_SolidShader;
        std::unique_ptr<Shader> m_PhongShader;
        std::unique_ptr<Shader> m_ShadowMappingShader;

        // Lights
        std::vector<PointLight*> m_PointLights;
        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<SpotLight*> m_SpotLights;

        // These vectors are the same size as there counterparts above that actually store the lights, however these
        // vectors store the global index (as per the Scene) for each light
        std::vector<size_t> m_PointLightIndices;
        std::vector<size_t> m_DirectionalLightIndices;
        std::vector<size_t> m_SpotLightIndices;

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

        // Shadow Maps
        unsigned int m_ShadowMapFBO;

        unsigned int m_ShadowMapTexture;

        const int m_ShadowMapWidth{ 1024 };
        const int m_ShadowMapHeight{ 1024 };
	};
}