#pragma once

namespace Rutile {
    enum class RendererType {
        OPENGL_SOLID_SHADING,
        OPENGL_PHONG_SHADING,
        CPU_RAY_TRACING,
        GPU_RAY_TRACING
    };
}