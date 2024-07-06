#include "ImGuiUtil.h"
#include "imgui.h"

#include "Settings/App.h"

namespace Rutile {
    void RadioButtons(const std::string& name, std::vector<std::string> optionNames, int* setting, void (Renderer::*function)()) {
        ImGui::Text(name.c_str());
        int i = 0;
        for (auto optionName : optionNames) {
            if (ImGui::RadioButton(optionName.c_str(), setting, i)) {
                if (function != nullptr) {
                    (App::renderer.get()->*function)();
                }
            }

            ++i;
        }
    }
}