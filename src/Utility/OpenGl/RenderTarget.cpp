#include "RenderTarget.h"

namespace Rutile {
	RenderTarget::RenderTarget(const glm::ivec2& size) 
		: size(size)
        , m_Framebuffer()
        , m_Renderbuffer(size)
        , m_Texture(size, TextureParameters{
            TextureFormat::RGB,
            TextureStorageType::UNSIGNED_BYTE,
            TextureWrapMode::REPEAT,
            TextureFilteringMode::LINEAR
        }) {

        m_Framebuffer.Bind();

        m_Texture.Bind();
        m_Framebuffer.AddTexture(m_Texture, Framebuffer::TextureUses::COLOR_0);

        m_Renderbuffer.Bind();
        m_Framebuffer.AddRenderbuffer(m_Renderbuffer, Framebuffer::RenderbufferUses::DEPTH_STENCIL);

        m_Framebuffer.Check("Renderer Buffer");

        m_Framebuffer.Unbind();
        m_Texture.Unbind();
        m_Renderbuffer.Unbind();
	}

	RenderTarget::~RenderTarget() { }

	void RenderTarget::Resize(const glm::ivec2& newSize) {

	}

    void RenderTarget::Bind() {
        m_Framebuffer.Bind();
    }

    void RenderTarget::Unbind() {
        m_Framebuffer.Unbind();
    }
}