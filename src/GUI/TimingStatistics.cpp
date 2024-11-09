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

        const auto averageFrameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(App::timingData.rollingAverageFrameTime);
        ImGui::Text(("Rolling Average Frame Time: " + std::to_string((double)averageFrameTime.count() / 1000000.0) + "ms").c_str());

        if (ImGui::DragInt("Length of rolling average", (int*)&App::timingData.rollingAverageLength, 1, 1, 10000)) {
            App::timingData.frameTimes.clear();
        }

        App::renderer->ProvideTimingStatistics();
    }
}