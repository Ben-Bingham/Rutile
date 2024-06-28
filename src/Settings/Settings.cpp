#include "Settings.h"

namespace Rutile {
    Settings DefaultSettings() {
        return Settings{ };
    }

    Settings ShadowMapTestingSceneSettings() {
        Settings settings{ };

        settings.shadowMapBiasMode = SHADOW_MAP_BIAS_MODE_STATIC;
        settings.shadowMapBias = 0.001f;
        settings.culledFaceDuringShadowMapping = GeometricFace::BACK;

        return settings;
    }
}