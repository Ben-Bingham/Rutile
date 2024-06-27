#pragma once
#include "Scene.h"

namespace Rutile {
    enum class Scenes {
        TRIANGLE_SCENE,
        ORIGINAL_SCENE
    };

    // The scene manager simply gives you a scene, and gives a home to all of the objects inside that scene.
    class SceneManager {
    public:
        static Scene GetScene(Scenes scene);

    private:
        static Material* GetMaterial(MaterialType type);
        static Light* GetLight(LightType type);
        static Transform* GetTransform();

        static Scene GetTriangleScene();
        static Scene GetOriginalScene();

        static inline std::vector<Light*> m_Lights;
        static inline std::vector<LightType> m_LightTypes;

        static inline std::vector<Transform*> m_Transforms;

        static inline std::vector<Material*> m_Materials;
        static inline std::vector<MaterialType> m_MaterialTypes;
    };
}