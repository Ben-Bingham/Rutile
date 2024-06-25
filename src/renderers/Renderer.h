#pragma once

#include "Bundle.h"
#include <vector>

#include "rendering/Camera.h"

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

        virtual void Init() = 0;
        virtual void Cleanup() = 0;

        virtual std::vector<Pixel> Render(const Camera& camera, const glm::mat4& projection) = 0;

        virtual void SetSize(size_t width, size_t height) = 0;
        virtual void SetBundle(const Bundle& bundle) = 0;
    };
}