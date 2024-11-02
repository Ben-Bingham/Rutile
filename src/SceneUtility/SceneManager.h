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
        HOLLOW_GLASS_SPHERE,
        RAY_TRACING_IN_ONE_WEEKEND,
        CORNELL_BOX,
        BACKPACK,
        CORNELL_BOX_VERSION_2,
        DRAGON_8K,
        DRAGON_80K,
        DRAGON_800K,
        SPORTS_CAR_FRONT,
        MINECRAFT_WORLD
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
        static Scene GetRayTracingInOneWeekendScene();
        static Scene GetCornellBoxScene();
        static Scene GetBackpackScene();
        static Scene GetCornellBoxVersion2();
        static Scene GetDragon8K();
        static Scene GetDragon80K();
        static Scene GetDragon800K();
        static Scene GetSportsCarFront();
        static Scene GetMinecraftWorld();
    };
}