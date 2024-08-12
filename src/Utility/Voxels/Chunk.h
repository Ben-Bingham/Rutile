#pragma once
#include <array>

namespace Rutile {
    struct Chunk {
        static constexpr size_t materialCount = 16;

        std::array<std::array<unsigned char, 64>, materialCount> blockData;
    };
}