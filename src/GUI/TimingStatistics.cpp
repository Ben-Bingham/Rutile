#include "TimingStatistics.h"
#include "imgui.h"
#include <chrono>

#include "Settings/App.h"

namespace Rutile {
    void TimingStatistics() {
        const auto frameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(App::timingData.frameTime);
        ImGui::Text(("Frame Time: " + std::to_string((double)frameTime.count() / 1000000.0) + "ms").c_str());

        const auto renderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(App::timingData.renderTime);
        ImGui::Text(("Render Time: " + std::to_string((double)renderTime.count() / 1000000.0) + "ms").c_str());

        const auto imGuiTime = std::chrono::duration_cast<std::chrono::nanoseconds>(App::timingData.imGuiTime);
        ImGui::Text(("ImGui Time: " + std::to_string((double)imGuiTime.count() / 1000000.0) + "ms").c_str());

        const auto averageFrameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - App::timingData.startTime);
        ImGui::Text(("Average Frame Time: " + std::to_string((double)averageFrameTime.count() / 1000000.0 / (double)App::frameCount) + "ms").c_str());

        App::renderer->ProvideTimingStatistics();
    }
}