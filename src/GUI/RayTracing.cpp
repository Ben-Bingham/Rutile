#include "RayTracing.h"
#include "imgui.h"

#include "Settings/App.h"

namespace Rutile {
    void RayTracingSettings() {
        if (ImGui::DragInt("Max Bounces", &App::settings.maxBounces, 0.1f)) { App::renderer->SignalRayTracingSettingsChange(); }
    }
}