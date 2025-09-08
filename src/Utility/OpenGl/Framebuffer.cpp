#include "Framebuffer.h"

#include <iostream>

namespace Rutile {
    Framebuffer::Framebuffer() {
        glGenFramebuffers(1, &m_FramebufferHandle);
        Bind();
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &m_FramebufferHandle);
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle);
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned Framebuffer::Get() {
        return m_FramebufferHandle;
    }

    void Framebuffer::AddTexture(Texture2D& texture, TextureUses use) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, (int)use, GL_TEXTURE_2D, texture.Get(), 0);
    }

    void Framebuffer::AddRenderbuffer(Renderbuffer& renderbuffer, RenderbufferUses use) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, (int)use, GL_RENDERBUFFER, renderbuffer.Get());
    }

    void Framebuffer::NoTargets() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    bool Framebuffer::Check(const std::string& framebufferName) {
        Bind();

        unsigned int result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (result != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR, Framebuffer: " << framebufferName << " is not complete, result is: " << result << std::endl;
        }

        return result;
    }
}