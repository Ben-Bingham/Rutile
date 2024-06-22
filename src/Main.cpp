#include <iostream>

#include "GeometryPreprocessor.h"
#include "renderers/HardCodedRenderer.h"
#include "renderers/renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

constexpr size_t width = 600;
constexpr size_t height = 400;

void init() {
    if (!glfwInit()) {
        std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
    }

    window = glfwCreateWindow(width, height, "Rutile", nullptr, nullptr);

    if (!window) {
        std::cout << "ERROR: Failed to create window." << std::endl;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
    }
}

void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        std::cout << "Looping" << std::endl;

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

using namespace Rutile;

int main() {
    std::unique_ptr<Renderer> renderer = std::make_unique<HardCodedRenderer>();

    renderer->Init();

    GeometryPreprocessor geometryPreprocessor{ };

    // geometryPreprocessor.add();
    Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

    std::vector<Pixel> pixels = renderer->Render(bundle, width, height);

    { // Rendering pixels to screen
        init();

        mainLoop();

        cleanup();
    }

    renderer->Cleanup();
}