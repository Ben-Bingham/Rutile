#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "rendering/Bundle.h"

namespace Rutile {
	class Renderer {
	public:
		Renderer() = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() = default;

        virtual GLFWwindow* Init() = 0;
        virtual void Cleanup(GLFWwindow* window) = 0;

        virtual void Render() = 0;

        virtual void SetBundle(const Bundle& bundle) = 0;

        virtual void WindowResize();
    };
}