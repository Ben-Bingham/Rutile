#pragma once
#include <string>
#include <chrono>

#include "imgui.h"

#include "Settings/App.h"

namespace Rutile {
    void RadioButtons(const std::string& name, std::vector<std::string> optionNames, int*, const std::function<void()>& func = []{ });
    
    std::string ChronoTimeToString(const std::chrono::duration<double>& time);
}