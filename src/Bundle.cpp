#include "Bundle.h"

namespace Rutile {
    void Clear(Bundle& bundle) {
        bundle.packets.clear();
        bundle.packetQuantities.clear();

        for (auto& vec : bundle.transforms) {
            vec.clear();
        }

        bundle.transforms.clear();
    }
}