#include "RenderTarget.h"

namespace Rutile {
	RenderTarget::RenderTarget(const glm::ivec2& size) 
		: m_Size(size)
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
        m_Size = newSize;

        m_Framebuffer.Bind();

        m_Texture.Bind();
        // TODO get these settings from the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture.Get(), 0);

        m_Renderbuffer.Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer.Get());

        m_Framebuffer.Unbind();
        m_Texture.Unbind();
        m_Renderbuffer.Unbind();
	}

    void RenderTarget::Bind() {
        m_Framebuffer.Bind();
        glViewport(0, 0, m_Size.x, m_Size.y);
    }

    void RenderTarget::Unbind() {
        m_Framebuffer.Unbind();
    }

    glm::ivec2 RenderTarget::GetSize() {
        return m_Size;
    }

    Texture2D& RenderTarget::GetTexture() {
        return m_Texture;
    }
}