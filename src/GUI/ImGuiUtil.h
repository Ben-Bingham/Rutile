#pragma once
#include <string>

#include "renderers/Renderer.h"

namespace Rutile {
    void RadioButtons(const std::string& name, std::vector<std::string> optionNames, int* setting, void (Renderer::* function)());
}