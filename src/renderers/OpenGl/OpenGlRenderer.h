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

        void Init(size_t width, size_t height) override;
        std::vector<Pixel> Render(const Bundle& bundle, const glm::mat4& view, const glm::mat4& projection, const Camera& camera) override;
        void Cleanup() override;

        void Resize(size_t width, size_t height) override;

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
    };
}