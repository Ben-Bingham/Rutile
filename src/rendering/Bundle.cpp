#include "Bundle.h"

namespace Rutile {
    void Clear(Bundle& bundle) {
        bundle.packets.clear();

        for (auto& vec : bundle.transforms) {
            vec.clear();
        }

        bundle.transforms.clear();

        bundle.lightTypes.clear();
        bundle.lights.clear();
    }
}