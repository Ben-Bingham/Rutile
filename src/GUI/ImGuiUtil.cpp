#include "ImGuiUtil.h"

namespace Rutile {
    void RadioButtons(const std::string& name, std::vector<std::string> optionNames, int* setting, const std::function<void()>& func) {
        bool sentEvent = false;
        ImGui::Text(name.c_str());
        int i = 0;
        for (auto optionName : optionNames) {
            if (ImGui::RadioButton(optionName.c_str(), setting, i)) {
                if (func != nullptr) {
                    func();
                }
            }

            ++i;
        }
    }

    std::string ChronoTimeToString(const std::chrono::duration<double>& time) {
        const auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(time);
        return std::to_string((double)t.count() / 1000000.0) + "ms";
    }
}