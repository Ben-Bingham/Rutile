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
                if (ImGui::Button("Restart Renderer")) {
                    App::restartRenderer = true;
                }
            }

            if (ImGui::CollapsingHeader("Local Renderer Options")) {}

            if (ImGui::CollapsingHeader("Scene Options")) {
                MainSceneGui();
            }

            /*
            if (ImGui::CollapsingHeader("Transforms")) {
                int transformCount = 1;
                int i = 0;
                for (auto t : App::scene.transforms) {
                    glm::mat4* transform = t[0];

                    if (ImGui::TreeNode(("Transform #" + std::to_string(transformCount)).c_str())) {
                        glm::vec3 translation = { (*transform)[3][0], (*transform)[3][1], (*transform)[3][2] };
                        glm::vec3 translationBackup = translation;
                        ImGui::DragFloat3(("Translation##" + std::to_string(i)).c_str(), glm::value_ptr(translation), 0.01f);
                        *transform = glm::translate(*transform, translation - translationBackup);

                        glm::vec3 scaling = { (*transform)[0][0], (*transform)[1][1], (*transform)[2][2] };
                        glm::vec3 scalingBackup = scaling - 1.0f;
                        ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), glm::value_ptr(scaling), 0.01f, 0.1f, 10000.0f);
                        *transform = glm::scale(*transform, scaling - scalingBackup);

                        ImGui::TreePop();
                    }
                    ++transformCount;
                    ++i;
                }
            }
            */
        }
        ImGui::End();
    }
}