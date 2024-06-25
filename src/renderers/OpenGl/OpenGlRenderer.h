#pragma once

#include "../Renderer.h"

namespace Rutile {
	class OpenGlRenderer : public Renderer {
	public:
        OpenGlRenderer() = default;
        OpenGlRenderer(const OpenGlRenderer& other) = default;
        OpenGlRenderer(OpenGlRenderer&& other) noexcept = default;
        OpenGlRenderer& operator=(const OpenGlRenderer& other) = default;
        OpenGlRenderer& operator=(OpenGlRenderer&& other) noexcept = default;
        ~OpenGlRenderer() override = default;

        void Init() override;
        void Cleanup() override;

        std::vector<Pixel> Render(const Camera& camera, const glm::mat4& projection) override;

        void SetSize(size_t width, size_t height) override;
        void SetBundle(const Bundle& bundle) override;

        enum RenderingMode {
            LOWEST_COMMON_MATERIAL,
            HIGHTEST_COMMON_MATERIAL,
            INDIVIDUAL_MATERIALS
        };

        RenderingMode renderingMode{ LOWEST_COMMON_MATERIAL };

    private:
        unsigned int m_SolidShader;
        unsigned int m_PhongShader;

        unsigned int m_FBO;
        unsigned int m_FBOTexture;
        unsigned int m_RBO;

        size_t m_Width;
        size_t m_Height;

        size_t m_PacketCount;

        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;

        std::vector<size_t> m_IndexCounts;
        std::vector<glm::mat4> m_Transforms;

        std::vector<MaterialType> m_MaterialTypes;
        std::vector<Material*> m_Materials;
	};
}