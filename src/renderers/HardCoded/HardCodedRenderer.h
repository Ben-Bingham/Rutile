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

        void Init() override;
        void Cleanup() override;

        std::vector<Pixel> Render(const Camera& camera, const glm::mat4& projection) override;

        void SetSize(size_t width, size_t height) override;
        void SetBundle(const Bundle& bundle) override;

    private:
        std::vector<Pixel> m_Pixels;
    };
}