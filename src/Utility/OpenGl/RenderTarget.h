#pragma once

#include <glm/glm.hpp>

#include "Framebuffer.h"
#include "Renderbuffer.h"
#include "Texture.h"

namespace Rutile {
	class RenderTarget {
	public:
		RenderTarget(const glm::ivec2& size);
		RenderTarget(const RenderTarget& other) = delete;
		RenderTarget(RenderTarget&& other) noexcept = default;
		RenderTarget& operator=(const RenderTarget& other) = delete;
		RenderTarget& operator=(RenderTarget&& other) noexcept = default;
		~RenderTarget();

		void Resize(const glm::ivec2& newSize);

		void Bind();
		void Unbind();

		glm::ivec2 GetSize();
		Texture2D& GetTexture();

	private:
		glm::ivec2 m_Size;

		Framebuffer m_Framebuffer;
		Renderbuffer m_Renderbuffer;
		Texture2D m_Texture;
	};
}