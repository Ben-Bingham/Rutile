#pragma once

#include <vector>

#include "rendering/Light.h"
#include "rendering/Object.h"

namespace Rutile {
    using ObjectIndex = size_t;
    using LightIndex = size_t;

    struct Scene {
        friend class SceneFactory;

        std::vector<Object> objects;

        std::vector<PointLight> pointLights;

        DirectionalLight directionalLight;

        bool HasDirectionalLight();

    private:
        bool m_EnableDirectionalLight{ false };
    };
}