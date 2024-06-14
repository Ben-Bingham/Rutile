#include "OpenGlRenderer.h"

#include <iostream>

namespace Rutile {
    void OpenGlRenderer::Init() {
        std::cout << "Initializing openGlRenderer" << std::endl;
    }

    std::vector<Pixel> OpenGlRenderer::Render(const Bundle& bundle, size_t width, size_t height) {
        std::cout << "Rendering with openGlRenderer" << std::endl;

        return std::vector<Pixel>{ };
    }

    void OpenGlRenderer::Cleanup() {
        std::cout << "Cleaning up openGlRenderer" << std::endl;
    }
}