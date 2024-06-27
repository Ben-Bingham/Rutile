#include "MainGuiWindow.h"

#include "GlobalRendererOptionsGui.h"

#include "imgui.h"

#include "Scenes/SceneGui.h"


namespace Rutile {
    void MainGuiWindow() {
        ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        ImGui::Begin("Rutile");
        {
            if (ImGui::CollapsingHeader("Global Renderer Options")) {
                GlobalRendererOptionsGui();
            }

            if (ImGui::CollapsingHeader("Local Renderer Options")) {}

            if (ImGui::CollapsingHeader("Scene Options")) {
                MainSceneGui();
            }
        }
        ImGui::End();
    }
}