#pragma once

#include <glm/glm.hpp>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
	class Window {
	public:
		Window(glm::ivec2 size);

		void Init();
		void Cleanup();

		GLFWwindow* Get();

		glm::ivec2 size{ };

	private:
		GLFWwindow* m_Window;
	};
}