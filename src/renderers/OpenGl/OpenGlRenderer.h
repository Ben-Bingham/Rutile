#pragma once

#include "../Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

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

        void SetPacket(size_t index, Packet packet) override;
        void SetLight(size_t index, LightType type, Light* light) override;

        void AddPacket(Packet packet) override;
        void AddLight(LightType type, Light* light) override;

        void UpdatePacketTransform(size_t index) override;

        void UpdateFieldOfView() override;
        void UpdateNearPlane() override;
        void UpdateFarPlane() override;

    private:
        void UpdateProjectionMatrix();

        glm::mat4 m_Projection { 1.0f };

        unsigned int m_SolidShader;
        unsigned int m_PhongShader;

        std::vector<PointLight*> m_PointLights;
        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<SpotLight*> m_SpotLights;

        size_t m_PacketCount;

        std::vector<bool> m_ValidPackets;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;

        std::vector<size_t> m_IndexCounts;
        std::vector<Transform*> m_Transforms;

        std::vector<MaterialType> m_MaterialTypes;
        std::vector<Material*> m_Materials;
	};
}