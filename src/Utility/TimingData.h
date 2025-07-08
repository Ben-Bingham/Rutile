#pragma once

namespace Rutile {
    struct TimingData {
        std::chrono::duration<double> frameTime;
        std::chrono::duration<double> renderTime;
    };
}