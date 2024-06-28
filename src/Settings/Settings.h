#pragma once
#include "RendererType.h"
#include <chrono>

namespace Rutile {
    struct Settings {
        float fieldOfView = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);

        RendererType defaultRenderer = RendererType::OPENGL;

        // 0 - No bias
        // 1 - Static Bias
        // 2 - Dynamic Bias
        int shadowMapBiasMode = 2;
        float shadowMapBias = 0.005f;

        float dynamicShadowMapBiasMin = 0.005f;
        float dynamicShadowMapBiasMax = 0.05f;
    };

    Settings DefaultSettings();
}