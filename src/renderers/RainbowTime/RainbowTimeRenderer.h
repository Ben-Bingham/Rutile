//#pragma once
//#include "../Renderer.h"
//
//#include <chrono>
//
//namespace Rutile {
//    class RainbowTimeRenderer : public Renderer {
//    public:
//        RainbowTimeRenderer() = default;
//        RainbowTimeRenderer(const RainbowTimeRenderer& other) = default;
//        RainbowTimeRenderer(RainbowTimeRenderer&& other) noexcept = default;
//        RainbowTimeRenderer& operator=(const RainbowTimeRenderer& other) = default;
//        RainbowTimeRenderer& operator=(RainbowTimeRenderer&& other) noexcept = default;
//        ~RainbowTimeRenderer() override = default;
//
//        void Init() override;
//        void Cleanup() override;
//
//        std::vector<Pixel> Render(const Camera& camera, const glm::mat4& projection) override;
//
//        void SetSize(size_t width, size_t height) override;
//        void SetBundle(const Bundle& bundle) override;
//
//    private:
//        std::chrono::system_clock::time_point m_StartTime{};
//
//        size_t m_Width;
//        size_t m_Height;
//    };
//}