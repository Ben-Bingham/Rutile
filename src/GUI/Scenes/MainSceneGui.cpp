#include "MainSceneGui.h"
#include "imgui.h"
#include <iostream>

#include "Settings/App.h"

namespace Rutile {
    void MainSceneGui() {
        const char* original = "Original";
        const char* triangle = "Triangle";

        const char* items[] = { original, triangle };
        static int currentIndex = 0;

        if (ImGui::BeginCombo("Select a Scene", items[currentIndex])) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool isSelected = (currentIndex == n);

                if (ImGui::Selectable(items[n], isSelected)) {
                    currentIndex = n;

                    if (std::string{ items[currentIndex] } == std::string{ original }) {
                        App::currentSceneType = SceneType::ORIGINAL_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ triangle }) {
                        App::currentSceneType = SceneType::TRIANGLE_SCENE;
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}