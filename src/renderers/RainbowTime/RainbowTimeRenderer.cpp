#include "RainbowTimeRenderer.h"

namespace Rutile {
    void RainbowTimeRenderer::Init(size_t width, size_t height) {
        m_Width = width;
        m_Height = height;
        m_StartTime = std::chrono::system_clock::now();
    }

    std::vector<Pixel> RainbowTimeRenderer::Render(const Bundle& bundle, const glm::mat4& view, const glm::mat4& projection) {
        std::vector<Pixel> pixels{ };

        pixels.reserve(m_Width * m_Height);

        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = currentTime - m_StartTime;

        const Byte r = (Byte)(std::cos(elapsedSeconds.count()) * 255.0f);
        const Byte g = (Byte)(std::sin(elapsedSeconds.count()) * 255.0f);
        const Byte b = (Byte)(std::tan(elapsedSeconds.count()) * 255.0f);

        for (size_t x = 0; x < m_Width; ++x) {
            for (size_t y = 0; y < m_Height; ++y) {
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

    void RainbowTimeRenderer::Cleanup() { }

    void RainbowTimeRenderer::Resize(size_t width, size_t height) {
        m_Width = width;
        m_Height = height;
    }
}