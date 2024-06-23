#pragma once
#include <unordered_map>

#include "../Renderer.h"

namespace Rutile {
    class HardCodedRenderer : public Renderer {
    public:
        HardCodedRenderer() = default;
        HardCodedRenderer(const HardCodedRenderer& other) = default;
        HardCodedRenderer(HardCodedRenderer&& other) noexcept = default;
        HardCodedRenderer& operator=(const HardCodedRenderer& other) = default;
        HardCodedRenderer& operator=(HardCodedRenderer&& other) noexcept = default;
        ~HardCodedRenderer() override = default;

        void Init(size_t width, size_t height) override;
        std::vector<Pixel> Render(const Bundle& bundle) override;
        void Cleanup() override;

        void Resize(size_t width, size_t height) override;

    private:
        std::vector<Pixel> m_Pixels;
    };
}