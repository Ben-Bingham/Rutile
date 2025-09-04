#include "MainGuiWindow.h"
#include "GeneralSettings.h"

#include "imgui.h"
#include "RayTracing.h"
#include "RenderingSettings.h"
#include "ShadowSettings.h"

#include "SceneObjects.h"
#include "TimingStatistics.h"

#include "Settings/App.h"

namespace Rutile {
    void MainGuiWindow() {
        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        ImGui::Begin("Rutile");
        {
            if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen)) {
                GeneralSettings();
            }

            if (ImGui::CollapsingHeader("Timing Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
                TimingStatistics();
            }

            if (ImGui::CollapsingHeader("Rendering Settings")) {
                RenderingSettings();
            }

            if (App::currentRendererType == RendererType::CPU_RAY_TRACING || App::currentRendererType == RendererType::GPU_RAY_TRACING || App::currentRendererType == RendererType::VOXEL_RAY_TRACING) {
                if (ImGui::CollapsingHeader("Ray Tracing Settings")) {
                    RayTracingSettings();
                }
            }

            if (ImGui::CollapsingHeader("Local Renderer Settings")) {
                //App::renderer->ProvideLocalRendererSettings();
            }

            //if (App::settings.materialType != MaterialType::SOLID && App::currentRendererType == RendererType::OPENGL) {
            //    if (ImGui::CollapsingHeader("Shadows")) {
            //        ShadowSettings();
            //    }
            //}

            if (ImGui::CollapsingHeader("Scene Objects")) {
                SceneObjects();
            }
        }
        ImGui::End();
    }
}