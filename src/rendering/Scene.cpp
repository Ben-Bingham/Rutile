#include "Scene.h"

namespace Rutile {
    void Clear(Scene& scene) {
        scene.packets.clear();

        scene.lightTypes.clear();
        scene.lights.clear();
    }
}