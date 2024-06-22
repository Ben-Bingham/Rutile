#include "HardCodedRenderer.h"

#include <iostream>

namespace Rutile {
    void HardCodedRenderer::Init() {
        std::cout << "Initializing hardCodedRenderer" << std::endl;
    }

    std::vector<Pixel> HardCodedRenderer::Render(const Bundle& bundle, size_t width, size_t height) {
        std::vector<Pixel> pixels{ };

        pixels.reserve(width * height);

        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
                Pixel pixel;
                Byte* bytePixel = (Byte*)&pixel;

                bytePixel[0] = 255;
                bytePixel[1] = 0;
                bytePixel[2] = 0;
                bytePixel[3] = 255;

                pixels.push_back(pixel);
            }
        }

        return std::vector<Pixel>{ };
    }

    void HardCodedRenderer::Cleanup() {
        std::cout << "Cleaning up hardCodedRenderer" << std::endl;
    }
}