#pragma once

#include <vector>

#include "Light.h"
#include "Object.h"

namespace Rutile {
    using ObjectIndex = size_t;
    using LightIndex = size_t;

    struct Scene {
        friend class SceneFactory;

        std::vector<Object> objects;

        std::vector<PointLight> pointLights;

        DirectionalLight directionalLight;

        bool HasDirectionalLight();

        GeometryBank geometryBank{ };
        MaterialBank materialBank{ };
        TransformBank transformBank{ };

    private:
        bool m_EnableDirectionalLight{ false };
    };
}