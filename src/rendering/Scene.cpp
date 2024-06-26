#include "Scene.h"

namespace Rutile {
    void Clear(Scene& bundle) {
        bundle.packets.clear();

        for (auto& vec : bundle.transforms) {
            vec.clear();
        }

        bundle.transforms.clear();

        bundle.lightTypes.clear();
        bundle.lights.clear();
    }
}