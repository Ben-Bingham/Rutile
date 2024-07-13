#include "Random.h"
#include <random>

namespace Rutile {
    float RandomFloat() {
        static std::uniform_real_distribution distribution(0.0f, 1.0f);
        static std::mt19937 generator{ };
        return distribution(generator);
    }
}