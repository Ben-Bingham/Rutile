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
        std::vector<Pixel> Render(const Bundle& bundle, size_t width, size_t height) override;
        void Cleanup() override;

    private:
        struct PairHash {
            template <class T1, class T2>
            std::size_t operator() (const std::pair<T1, T2>& p) const {
                auto hash1 = std::hash<T1>{}(p.first);
                auto hash2 = std::hash<T2>{}(p.second);
                return hash1 ^ (hash2 << 1);
            }
        };

        std::unordered_map<std::pair<size_t, size_t>, std::vector<Pixel>, PairHash> m_PixelMap;
    };
}