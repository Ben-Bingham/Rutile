#include "MainGuiWindow.h"
#include "imgui.h"
#include <iostream>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "Scenes/SceneGui.h"

#include "Settings/App.h"

namespace Rutile {
    void MainGuiWindow() {
        ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        ImGui::Begin("Rutile");
        {
            if (ImGui::CollapsingHeader("Global Renderer Options")) {
                std::string rendererTypeName{ };
                switch (App::currentRendererType) {
                    case RendererType::OPENGL: {
                        rendererTypeName = "OpenGl Renderer";
                        break;
                    }
                }

                ImGui::Text(("Current Renderer: " + rendererTypeName).c_str());

                ImGui::Separator();

                if (ImGui::Button("Restart Renderer")) {
                    App::restartRenderer = true;
                }
            }

            if (ImGui::CollapsingHeader("Local Renderer Options")) {}

            if (ImGui::CollapsingHeader("Scene Options")) {
                MainSceneGui();
            }
        }
        ImGui::End();
    }
}