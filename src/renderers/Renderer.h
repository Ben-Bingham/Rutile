#pragma once

#include "Bundle.h"
#include <vector>

namespace Rutile {
    using Pixel = uint32_t;
    using Byte = uint8_t;

	class Renderer {
	public:
		Renderer() = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() = default;

        virtual void Init(size_t width, size_t height) = 0;
        virtual std::vector<Pixel> Render(const Bundle& bundle, const glm::mat4& view, const glm::mat4& projection) = 0;
        virtual void Cleanup() = 0;

        virtual void Resize(size_t width, size_t height) = 0;
    };
}