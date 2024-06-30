#include "Settings.h"

namespace Rutile {
    Settings DefaultSettings() {
        return Settings{ };
    }

    Settings ShadowMapTestingSceneSettings() {
        Settings settings{ };

        settings.shadowMapBiasMode = ShadowMapBiasMode::STATIC;
        settings.shadowMapBias = 0.0015f;
        settings.culledFaceDuringShadowMapping = GeometricFace::BACK;
        settings.shadowMapPcfMode = ShadowMapPCFMode::BASIC;
        settings.shadowMapMode = ShadowMapMode::ONE_SHADOW_EMITTER;

        settings.omnidirectionalShadowMapPcfMode = OmnidirectionalShadowMapPCFMode::STANDARD;

        return settings;
    }
}