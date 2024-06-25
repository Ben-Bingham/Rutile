#include "HardCodedRenderer.h"

namespace Rutile {
    std::vector<Pixel> CreatePixels(size_t width, size_t height) {
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

        return pixels;
    }

    void HardCodedRenderer::Init(size_t width, size_t height) {
        m_Pixels = CreatePixels(width, height);
    }

    std::vector<Pixel> HardCodedRenderer::Render(const Bundle& bundle, const glm::mat4& view, const glm::mat4& projection, const Camera& camera) {
        return m_Pixels;
    }

    void HardCodedRenderer::Cleanup() { }

    void HardCodedRenderer::Resize(size_t width, size_t height) {
        m_Pixels = CreatePixels(width, height);
    }
}