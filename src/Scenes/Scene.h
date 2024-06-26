#pragma once

#include <vector>

#include "rendering/Light.h"
#include "rendering/Packet.h"

namespace Rutile {
    struct Scene {
        std::vector<Packet> packets;

        std::vector<LightType> lightTypes;
        std::vector<Light*> lights;
    };

    void Clear(Scene& scene);
}