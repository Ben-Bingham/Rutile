#include "HardCodedRenderer.h"

#include <iostream>

namespace Rutile {
    void HardCodedRenderer::Init() {
        std::cout << "Initializing hardCodedRenderer" << std::endl;
    }

    std::vector<Pixel> HardCodedRenderer::Render(const Bundle& bundle, size_t width, size_t height) {
        std::pair<size_t, size_t> key = std::make_pair(width, height);

        if (!m_PixelMap.contains(key)) {
            std::vector<Pixel> pixels{ };

            pixels.reserve(width * height);

            for (size_t x = 0; x < width; ++x) {
                for (size_t y = 0; y < height; ++y) {
                    Pixel pixel = 0;
                    Byte* bytePixel = (Byte*)&pixel;

                    bytePixel[0] = 255;
                    bytePixel[1] = 255;
                    bytePixel[2] = 0;
                    bytePixel[3] = 255;

                    pixels.push_back(pixel);
                }
            }

            m_PixelMap[key] = pixels;
        }

        return m_PixelMap[key];
    }

    void HardCodedRenderer::Cleanup() {
        std::cout << "Cleaning up hardCodedRenderer" << std::endl;
    }
}