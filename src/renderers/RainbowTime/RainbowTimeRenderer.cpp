#include "RainbowTimeRenderer.h"

#include <iostream>

namespace Rutile {
    void RainbowTimeRenderer::Init() {
        std::cout << "Initializing rainbowTimeRenderer" << std::endl;
        m_StartTime = std::chrono::system_clock::now();
    }

    std::vector<Pixel> RainbowTimeRenderer::Render(const Bundle& bundle, size_t width, size_t height) {
        std::vector<Pixel> pixels{ };

        pixels.reserve(width * height);

        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = currentTime - m_StartTime;

        const Byte r = (Byte)(std::cos(elapsedSeconds.count()) * 255.0f);
        const Byte g = (Byte)(std::sin(elapsedSeconds.count()) * 255.0f);
        const Byte b = (Byte)(std::tan(elapsedSeconds.count()) * 255.0f);

        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
                Pixel pixel = 0;
                Byte* bytePixel = (Byte*)&pixel;

                bytePixel[0] = r;
                bytePixel[1] = g;
                bytePixel[2] = b;
                bytePixel[3] = 255;

                pixels.push_back(pixel);
            }
        }

        return pixels;
    }

    void RainbowTimeRenderer::Cleanup() {
        std::cout << "Cleaning up rainbowTimeRenderer" << std::endl;
    }
}