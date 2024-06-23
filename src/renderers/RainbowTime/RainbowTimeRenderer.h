#pragma once
#include "../Renderer.h"

#include <chrono>

namespace Rutile {
    class RainbowTimeRenderer : public Renderer {
    public:
        RainbowTimeRenderer() = default;
        RainbowTimeRenderer(const RainbowTimeRenderer& other) = default;
        RainbowTimeRenderer(RainbowTimeRenderer&& other) noexcept = default;
        RainbowTimeRenderer& operator=(const RainbowTimeRenderer& other) = default;
        RainbowTimeRenderer& operator=(RainbowTimeRenderer&& other) noexcept = default;
        ~RainbowTimeRenderer() override = default;

        void Init(size_t width, size_t height) override;
        std::vector<Pixel> Render(const Bundle& bundle, const glm::mat4& view, const glm::mat4& projection) override;
        void Cleanup() override;

        void Resize(size_t width, size_t height) override;

    private:
        std::chrono::system_clock::time_point m_StartTime{};

        size_t m_Width;
        size_t m_Height;
    };
}