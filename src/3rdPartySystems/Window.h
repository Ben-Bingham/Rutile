#pragma once

#include <glm/glm.hpp>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
	class Window {
	public:
		Window(glm::ivec2 size);
		Window(const Window& other) = delete;
		Window(Window&& other) noexcept = default;
		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) noexcept = default;
		~Window();

		GLFWwindow* Get();

		bool IsOpen();

		glm::ivec2 size{ };

	private:
		GLFWwindow* m_Window;
	};
}