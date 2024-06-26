//#include "HardCodedRenderer.h"
//
//namespace Rutile {
//    void HardCodedRenderer::Init() { }
//
//    void HardCodedRenderer::Cleanup() { }
//
//    std::vector<Pixel> HardCodedRenderer::Render(const Camera& camera, const glm::mat4& projection) {
//        return m_Pixels;
//    }
//
//    void HardCodedRenderer::SetSize(size_t width, size_t height) {
//        m_Pixels.clear();
//        m_Pixels.reserve(width * height);
//
//        for (size_t x = 0; x < width; ++x) {
//            for (size_t y = 0; y < height; ++y) {
//                Pixel pixel = 0;
//                Byte* bytePixel = (Byte*)&pixel;
//
//                bytePixel[0] = 255;
//                bytePixel[1] = 255;
//                bytePixel[2] = 0;
//                bytePixel[3] = 255;
//
//                m_Pixels.push_back(pixel);
//            }
//        }
//    }
//
//    void HardCodedRenderer::SetBundle(const Bundle& bundle) { }
//}