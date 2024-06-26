#pragma once

#include "../Renderer.h"

#include <GLFW/glfw3native.h>

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

        std::vector<Pixel> Render(const Camera& camera, const glm::mat4& projection) override;

        void SetBundle(const Bundle& bundle) override;

        void WindowResize() override;

    private:
        unsigned int m_SolidShader;
        unsigned int m_PhongShader;

        std::vector<PointLight*> m_PointLights;
        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<SpotLight*> m_SpotLights;

        size_t m_PacketCount;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;

        std::vector<size_t> m_IndexCounts;
        std::vector<glm::mat4*> m_Transforms;

        std::vector<MaterialType> m_MaterialTypes;
        std::vector<Material*> m_Materials;
	};
}