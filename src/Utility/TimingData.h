#pragma once

namespace Rutile {
    struct TimingData {
        std::chrono::time_point<std::chrono::steady_clock> startTime;

        std::chrono::duration<double> frameTime;
        std::chrono::duration<double> renderTime;
        std::chrono::duration<double> imGuiTime;

        std::vector<std::chrono::duration<double>> frameTimes;
        std::chrono::duration<double> rollingAverageFrameTime;

        size_t rollingAverageLength{ 5 };
    };
}