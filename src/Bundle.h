#pragma once

#include <vector>

#include "Packet.h"

namespace Rutile {
    struct Bundle {
        std::vector<Packet> packets;
        std::vector<uint32_t> packetQuantities;

        //std::vector<std::vector<glm::mat4>> transforms;
        //std::vector<Light> lights;
        //Camera camera;
    };
}