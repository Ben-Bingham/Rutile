#pragma once
#include "Scene.h"

namespace Rutile {
    enum class SceneType {
        TRIANGLE_SCENE,
        ORIGINAL_SCENE,
        SHADOW_MAP_TESTING_SCENE,
        OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE,
        DOUBLE_POINT_LIGHT_TEST_SCENE,
        ALL_SPHERES,
        SPHERES_ON_SPHERES
    };

    // The scene manager simply gives you a scene, and gives a home to all of the objects inside that scene.
    class SceneManager {
    public:
        static Scene GetScene(SceneType scene);

    private:
        static Phong GetPhong(const Solid& solid);
        static Solid GetSolid(const Phong& phong);

        static Scene GetTriangleScene();
        static Scene GetOriginalScene();
        static Scene GetShadowMapTestingScene();
        static Scene GetOmnidirectionalShadowMapTestingScene();
        static Scene GetDoublePointLightTestScene();
        static Scene GetAllSpheresScene();
        static Scene GetSpheresOnSpheresScene();
    };
}