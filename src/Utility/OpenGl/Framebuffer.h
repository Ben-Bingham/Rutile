#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Texture.h"
#include "Renderbuffer.h"

namespace Rutile {
    class Framebuffer {
    public:
        enum class TextureUses {
            COLOR_0 = GL_COLOR_ATTACHMENT0
        };

        enum class RenderbufferUses {
            DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT
        };

        Framebuffer();
        Framebuffer(const Framebuffer& other) = delete;
        Framebuffer(Framebuffer&& other) noexcept = default;
        Framebuffer& operator=(const Framebuffer& other) = delete;
        Framebuffer& operator=(Framebuffer&& other) noexcept = default;
        ~Framebuffer();

        void Bind();
        void Unbind();

        unsigned int Get();

        // TODO potentially make these get passed to the constructor
        void AddTexture(Texture2D& texture, TextureUses use);
        void AddRenderbuffer(Renderbuffer& renderbuffer, RenderbufferUses use);

        void NoTargets();

        bool Check(const std::string& framebufferName);

    private:
        unsigned int m_FramebufferHandle;
    };
}