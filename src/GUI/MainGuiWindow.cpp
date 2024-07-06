#include "MainGuiWindow.h"
#include "GeneralSettings.h"

#include "imgui.h"
#include "RenderingSettings.h"
#include "ShadowSettings.h"

#include "SceneObjects.h"

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

            if (ImGui::CollapsingHeader("Rendering Settings")) {
                RenderingSettings();
            }

            if (App::settings.materialType != MaterialType::SOLID) {
                if (ImGui::CollapsingHeader("Shadows")) {
                    ShadowSettings();
                }
            }

            if (ImGui::CollapsingHeader("Scene Objects")) {
                SceneObjects();
            }
        }
        ImGui::End();
    }
}