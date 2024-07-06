#pragma once

namespace Rutile {
    enum class ShadowMapBiasMode {
        NONE,
        STATIC,
        DYNAMIC
    };

    enum class OmnidirectionalShadowMapPCFMode {
        NONE,
        FIXED_SAMPLE_COUNT,
        FIXED_SAMPLE_DIRECTIONS
    };

    enum class OmnidirectionalShadowMapDiskRadiusMode {
        STATIC,
        DYNAMIC
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