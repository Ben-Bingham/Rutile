#pragma once
#include <string>

#include "imgui.h"

#include "Settings/App.h"

namespace Rutile {
    void RadioButtons(const std::string& name, std::vector<std::string> optionNames, int*, const std::function<void()>& func = []{ });
}