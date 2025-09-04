#include "Window.h"

#include <iostream>

namespace Rutile {
	Window::Window(glm::ivec2 size) 
		: size(size) {

	}

	void Window::Init() {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		m_Window = glfwCreateWindow(size.x, size.y, "Rutile", nullptr, nullptr);

		if (!m_Window) {
			std::cout << "ERROR: Failed to create window." << std::endl;
		}

		glfwMakeContextCurrent(m_Window);
	}

	void Window::Cleanup() {

	}

	GLFWwindow* Window::Get() {
		return m_Window;
	}
}