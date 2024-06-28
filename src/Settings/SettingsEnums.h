#pragma once

namespace Rutile {
    enum class ShadowMapBiasMode {
        NONE,
        STATIC,
        DYNAMIC
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