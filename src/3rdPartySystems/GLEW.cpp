#include "GLEW.h"

#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
	void GLEW::Init() {
		if (glewInit() != GLEW_OK) {
			std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
		}
	}

	void GLEW::Cleanup() { }
}