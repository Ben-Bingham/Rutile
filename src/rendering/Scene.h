#pragma once

#include "Light.h"
#include <vector>

#include "Packet.h"

namespace Rutile {
    struct Scene {
        std::vector<Packet> packets;

        std::vector<std::vector<glm::mat4*>> transforms;

        std::vector<LightType> lightTypes;
        std::vector<Light*> lights;
    };

    void Clear(Scene& bundle);
}