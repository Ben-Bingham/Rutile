#include "RenderingSettings.h"
#include "imgui.h"
#include "ImGuiUtil.h"

#include <glm/gtc/type_ptr.hpp>

#include "Settings/App.h"

#include "utility/events/Events.h"

namespace Rutile {
    void RenderingSettings() {
        if (App::currentRendererType != RendererType::CPU_RAY_TRACING) {
            RadioButtons("Front Face Winding Order", { "Clock-Wise##rs", "Counter-Clock-Wise##rs" }, (int*)&App::settings.frontFace, nullptr);

            ImGui::Separator();

            RadioButtons("Culled Face During Rendering", { "Front##rs", "Back##rs" }, (int*)&App::settings.culledFaceDuringRendering, nullptr);

            ImGui::Separator();
        }

        ImGui::Text("View Frustum");
        if (ImGui::DragFloat("Field of View",       &App::settings.fieldOfView, 0.1f, 0.0f, 180.0f))      { App::renderer->ProjectionMatrixUpdate(); }
        if (ImGui::DragFloat("Near Clipping Plane", &App::settings.nearPlane,   0.1f, 0.0f, 10000000.0f)) { App::renderer->ProjectionMatrixUpdate(); }
        if (ImGui::DragFloat("Far Clipping Plane",  &App::settings.farPlane,    0.1f, 0.0f, 10000000.0f)) { App::renderer->ProjectionMatrixUpdate(); }

        ImGui::Separator();

        ImGui::Text("Camera");
        if (ImGui::DragFloat3("Position##camera",           glm::value_ptr(App::camera.position),    0.1f                  )) { App::eventManager.Notify(new CameraUpdate{ }); App::updateCameraVectors = true; }

        if (ImGui::DragFloat ("Yaw##camera",                &App::camera.yaw,                        0.1f                  )) { App::eventManager.Notify(new CameraUpdate{ }); App::updateCameraVectors = true; }
        if (ImGui::DragFloat ("Pitch##camera",              &App::camera.pitch,                      0.1f,  -89.9f, 89.9f  )) { App::eventManager.Notify(new CameraUpdate{ }); App::updateCameraVectors = true; }

        if (ImGui::DragFloat ("Mouse Sensitivity##camera",  &App::camera.lookSensitivity,            0.01f, 0.0f,   100.0f )) { App::eventManager.Notify(new CameraUpdate{ }); }
        if (ImGui::DragFloat ("Movement Speed##camera",     &App::camera.speed,                      0.1f,  0.0f,   1000.0f)) { App::eventManager.Notify(new CameraUpdate{ }); }
    }
}