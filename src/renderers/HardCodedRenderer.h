#pragma once
#include "Renderer.h"

#include <chrono>

namespace Rutile {
    class HardCodedRenderer : public Renderer {
    public:
        HardCodedRenderer() = default;
        HardCodedRenderer(const HardCodedRenderer& other) = default;
        HardCodedRenderer(HardCodedRenderer&& other) noexcept = default;
        HardCodedRenderer& operator=(const HardCodedRenderer& other) = default;
        HardCodedRenderer& operator=(HardCodedRenderer&& other) noexcept = default;
        ~HardCodedRenderer() override = default;

        void Init() override;
        std::vector<Pixel> Render(const Bundle& bundle, size_t width, size_t height) override;
        void Cleanup() override;

    private:
        std::chrono::system_clock::time_point m_StartTime{};
    };
}