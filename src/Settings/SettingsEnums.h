#pragma once

namespace Rutile {
    enum class ShadowMapMode {
        NONE,
        ONE_SHADOW_EMITTER,
        CASCADING_SHADOW_MAPS
    };

    enum class ShadowMapBiasMode {
        NONE,
        STATIC,
        DYNAMIC
    };

    enum class ShadowMapPCFMode {
        NONE,
        BASIC
    };

    enum class OmnidirectionalShadowMapPCFMode {
        NONE,
        STANDARD,
        FIXED
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