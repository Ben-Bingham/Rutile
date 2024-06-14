#include <iostream>


#include "GeometryPreprocessor.h"
#include "renderers/OpenGlRenderer.h"
#include "renderers/renderer.h"

using namespace Rutile;

int main() {
    size_t width = 600;
    size_t height = 400;

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();

    renderer->Init();

    GeometryPreprocessor geometryPreprocessor{ };

    // geometryPreprocessor.add();
    Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

    renderer->Render(bundle, width, height);

    renderer->Cleanup();

    std::cin.get();
}