#pragma once

namespace Rutile {
    enum ShadowMapBiasMode {
        SHADOW_MAP_BIAS_MODE_NONE = 0,
        SHADOW_MAP_BIAS_MODE_STATIC = 1,
        SHADOW_MAP_BIAS_MODE_DYNAMIC = 2
    };

    enum class ShadowMapPCFMode {
        NONE,
        BASIC
    };

    enum class GeometricFace {
        FRONT,
        BACK
    };

    enum class WindingOrder {
        CLOCK_WISE,
        COUNTER_CLOCK_WISE
    };
}