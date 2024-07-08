#include "Settings.h"

namespace Rutile {
    Settings DefaultSettings() {
        return Settings{ };
    }

    Settings ShadowMapTestingSceneSettings() {
        Settings settings{ };

        settings.shadowMapBiasMode = ShadowMapBiasMode::STATIC;
        settings.materialType = MaterialType::PHONG;
        //settings.shadowMapBias = 0.0015f;
        //settings.culledFaceDuringShadowMapping = GeometricFace::BACK;
        //settings.shadowMapPcfMode = DirectionalShadowMapPCFMode::BASIC;
        //settings.shadowMapMode = ShadowMapMode::ONE_SHADOW_EMITTER;

        //settings.omnidirectionalShadowMapPCFMode = OmnidirectionalShadowMapPCFMode::STANDARD;

        return settings;
    }
}