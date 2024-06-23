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

        void Init() override;
        std::vector<Pixel> Render(const Bundle& bundle, size_t width, size_t height) override;
        void Cleanup() override;

    private:
        std::chrono::system_clock::time_point m_StartTime{};
    };
}