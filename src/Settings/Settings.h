#pragma once
#include "RendererType.h"
#include "SettingsEnums.h"
#include <chrono>

namespace Rutile {
    struct Settings {
        float fieldOfView = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);

        RendererType defaultRenderer = RendererType::OPENGL;

        // Rendering
        GeometricFace culledFaceDuringRendering = GeometricFace::BACK;

        WindingOrder frontFace = WindingOrder::COUNTER_CLOCK_WISE;

        // Shadow Maps
        ShadowMapMode shadowMapMode = ShadowMapMode::NONE;

        ShadowMapBiasMode shadowMapBiasMode = ShadowMapBiasMode::DYNAMIC;
        float shadowMapBias = 0.005f;

        float dynamicShadowMapBiasMin = 0.005f;
        float dynamicShadowMapBiasMax = 0.05f;

        ShadowMapPCFMode shadowMapPcfMode = ShadowMapPCFMode::NONE;

        GeometricFace culledFaceDuringShadowMapping = GeometricFace::FRONT;

        // Omnidirectional Shadow Maps
        float omnidirectionalShadowMapBias = 0.05f;
        OmnidirectionalShadowMapPCFMode omnidirectionalShadowMapPcfMode = OmnidirectionalShadowMapPCFMode::FIXED;

        float omnidirectionalShadowMapSamples = 4.0f;

        OmnidirectionalShadowMapDiskRadiusMode omnidirectionalShadowMapDiskRadiusMode = OmnidirectionalShadowMapDiskRadiusMode::DYNAMIC;
        float omnidirectionalShadowMapDiskRadius = 0.05f;
    };

    Settings DefaultSettings();

    Settings ShadowMapTestingSceneSettings();
}