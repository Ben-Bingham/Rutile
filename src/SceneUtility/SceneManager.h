#pragma once
#include "RenderingAPI/Scene.h"

namespace Rutile {
    enum class SceneType {
        TRIANGLE_SCENE,
        ORIGINAL_SCENE,
        SHADOW_MAP_TESTING_SCENE,
        OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE,
        DOUBLE_POINT_LIGHT_TEST_SCENE,
        ALL_SPHERES,
        SPHERES_ON_SPHERES,
        HOLLOW_GLASS_SPHERE
    };

    class SceneManager {
    public:
        static Scene GetScene(SceneType scene);

    private:
        static Scene GetTriangleScene();
        static Scene GetOriginalScene();
        static Scene GetShadowMapTestingScene();
        static Scene GetOmnidirectionalShadowMapTestingScene();
        static Scene GetDoublePointLightTestScene();
        static Scene GetAllSpheresScene();
        static Scene GetSpheresOnSpheresScene();
        static Scene GetHollowGlassSphereScene();
    };
}