#pragma once
#include "RendererType.h"
#include "SettingsEnums.h"
#include <chrono>

#include "rendering/Material.h"

namespace Rutile {
    struct Settings {
        MaterialType materialType = MaterialType::SOLID;

        float fieldOfView = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);

        RendererType defaultRenderer = RendererType::OPENGL;

        // Rendering
        GeometricFace culledFaceDuringRendering = GeometricFace::BACK;

        WindingOrder frontFace = WindingOrder::COUNTER_CLOCK_WISE;

        glm::vec3 backgroundColor = { 66.0f / 255.0f, 135.0f / 255.0f, 245.0f / 255.0f };

        // Directional Shadows
        bool directionalShadows = true;

        ShadowMapBiasMode shadowMapBiasMode = ShadowMapBiasMode::DYNAMIC;
        float directionalShadowMapBias = 0.005f;

        float directionalDynamicShadowMapBiasMin = 0.005f;
        float directionalDynamicShadowMapBiasMax = 0.05f;

        GeometricFace culledFaceDuringDirectionalShadowMapping = GeometricFace::FRONT;

        bool directionalShadowMapPCF = true;

        // Cascading Shadow maps
        bool visualizeCascades = false;
        bool visualizeCascadeLights = false;

        bool lockCascadeCamera = false;

        // Omnidirectional Shadow maps
        bool omnidirectionalShadowMaps = true;

        float omnidirectionalShadowMapBias = 0.05f;

        GeometricFace culledFaceDuringOmnidirectionalShadowMapping = GeometricFace::FRONT;

        OmnidirectionalShadowMapPCFMode omnidirectionalShadowMapPCFMode = OmnidirectionalShadowMapPCFMode::FIXED_SAMPLE_DIRECTIONS;

        int omnidirectionalShadowMapSampleCount = 4;

        OmnidirectionalShadowMapDiskRadiusMode omnidirectionalShadowMapDiskRadiusMode = OmnidirectionalShadowMapDiskRadiusMode::DYNAMIC;
        float omnidirectionalShadowMapDiskRadius = 0.05f;

        // Ray-Tracing
        int maxBounces = 5;
    };

    Settings DefaultSettings();

    Settings ShadowMapTestingSceneSettings();
}