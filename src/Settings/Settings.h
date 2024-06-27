#pragma once
#include "RendererType.h"
#include <chrono>

namespace Rutile {
    struct Settings {
        std::string name = "Rutile";

        float fieldOfView = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);

        RendererType defaultRenderer = RendererType::OPENGL;
    };

    Settings DefaultSettings();
}