#pragma once
#include "Event.h"
#include "RenderingAPI/Scene.h"

namespace Rutile {
    struct WindowResize : Event { };
    struct CameraUpdate : Event { };

    struct ObjectTransformUpdate : Event {
        ObjectTransformUpdate(ObjectIndex index)
            : index(index) {}

        ObjectIndex index;
    };

    struct ObjectMaterialUpdate : Event {
        ObjectMaterialUpdate(ObjectIndex index)
            : index(index) {}

        ObjectIndex index;
    };

    struct MaterialTypeUpdate : Event { };

    struct DirectionalShadowMapUpdate : Event { };
    struct OmnidirectionalShadowMapUpdate : Event { };
}