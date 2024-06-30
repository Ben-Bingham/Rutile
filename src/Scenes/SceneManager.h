#pragma once
#include "Scene.h"

namespace Rutile {
    enum class SceneType {
        TRIANGLE_SCENE,
        ORIGINAL_SCENE,
        SHADOW_MAP_TESTING_SCENE,
        MULTI_SHADOW_CASTER_SHADOW_MAP_TESTING_SCENE,
        OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE,
        GENERAL_SCENE
    };

    // The scene manager simply gives you a scene, and gives a home to all of the objects inside that scene.
    class SceneManager {
    public:
        static Scene GetScene(SceneType scene);

    private:
        static Material* GetMaterial(MaterialType type);
        static Light* GetLight(LightType type);
        static DirectionalLight* GetDirectionalLight();
        static Transform* GetTransform();

        static Scene GetTriangleScene();
        static Scene GetOriginalScene();
        static Scene GetShadowMapTestingScene();
        static Scene GetMultiLightShadowMapTestingScene();
        static Scene GetOmnidirectionalShadowMapTestingScene();
        static Scene GetGeneralScene();

        static inline std::vector<Light*> m_Lights;
        static inline std::vector<LightType> m_LightTypes;

        static inline std::vector<Transform*> m_Transforms;

        static inline std::vector<Material*> m_Materials;
        static inline std::vector<MaterialType> m_MaterialTypes;

        static inline DirectionalLight* m_DirectionalLight{ nullptr };
        static inline bool m_HaveCreatedDirectionalLight = false;
    };
}