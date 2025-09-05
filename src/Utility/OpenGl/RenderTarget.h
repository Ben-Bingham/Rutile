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

		glm::ivec2 size;

		void Resize(const glm::ivec2& newSize);

		void Bind();
		void Unbind();

	//private:
		Framebuffer m_Framebuffer;
		Renderbuffer m_Renderbuffer;
		Texture2D m_Texture;
	};
}