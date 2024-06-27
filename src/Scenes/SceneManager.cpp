#include "SceneManager.h"
#include "SceneFactory.h"

namespace Rutile {
    Scene SceneManager::GetScene(SceneType scene) {
        for (const Material* material : m_Materials) {
            delete material;
        }

        m_Materials.clear();
        m_MaterialTypes.clear();

        for (const Light* light : m_Lights) {
            delete light;
        }

        m_Lights.clear();
        m_LightTypes.clear();

        for (const Transform* transform : m_Transforms) {
            delete transform;
        }

        m_Transforms.clear();

        switch (scene) {
            case SceneType::TRIANGLE_SCENE: {
                return GetTriangleScene();
            }
            case SceneType::ORIGINAL_SCENE: {
                return GetOriginalScene();
            }
        }
    }

    Material* SceneManager::GetMaterial(MaterialType type) {
        switch (type) {
            case MaterialType::SOLID: {
                m_Materials.push_back(new Solid{ });
                m_MaterialTypes.push_back(MaterialType::SOLID);
                break;
            }
            case MaterialType::PHONG: {
                m_Materials.push_back(new Phong{ });
                m_MaterialTypes.push_back(MaterialType::PHONG);
                break;
            }
        }

        return m_Materials.back();
    }

    Light* SceneManager::GetLight(LightType type) {
        switch (type) {
            case LightType::POINT: {
                m_Lights.push_back(new PointLight{ });
                m_LightTypes.push_back(LightType::POINT);
                break;
            }
            case LightType::DIRECTION: {
                m_Lights.push_back(new DirectionalLight{ });
                m_LightTypes.push_back(LightType::DIRECTION);
                break;
            }
            case LightType::SPOTLIGHT: {
                m_Lights.push_back(new SpotLight{ });
                m_LightTypes.push_back(LightType::SPOTLIGHT);
                break;
            }
        }

        return m_Lights.back();
    }

    Transform* SceneManager::GetTransform() {
        m_Transforms.push_back(new Transform{ });

        return m_Transforms.back();
    }

    Scene SceneManager::GetTriangleScene() {
        SceneFactory sceneFactory{ };

        Transform* transform = GetTransform();
        Solid* solid = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        sceneFactory.Add(Primitive::TRIANGLE, transform, MaterialType::SOLID, solid);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetOriginalScene() {
        SceneFactory sceneFactory{ };

        Solid* solid = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        solid->color = { 1.0f, 0.0f, 1.0f };

        Solid* solid2 = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        solid2->color = { 0.2f, 0.5f, 0.7f };

        Phong* phong = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));

        phong->ambient = { 1.0f, 0.5f, 0.31f };
        phong->diffuse = { 1.0f, 0.5f, 0.31f };
        phong->specular = { 0.5f, 0.5f, 0.5f };
        phong->shininess = 32.0f;

        Transform* transform1 = GetTransform();
        transform1->position = { 1.0f, 1.0f, 0.0f };
        sceneFactory.Add(Primitive::TRIANGLE, transform1, MaterialType::SOLID, solid);

        Transform* transform2 = GetTransform();
        transform2->position = { -1.0f, -1.0f, 0.0f };
        sceneFactory.Add(Primitive::TRIANGLE, transform2, MaterialType::SOLID, solid2);

        Transform* transform3 = GetTransform();
        transform3->position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add(Primitive::SQUARE, transform3, MaterialType::SOLID, solid);

        Transform* transform4 = GetTransform();
        transform4->position = { 1.0f, -1.0f, 0.0f };
        sceneFactory.Add(Primitive::CUBE, transform4, MaterialType::SOLID, solid2);

        Transform* transform5 = GetTransform();
        transform5->position = { -1.0f, 1.0f, 0.0f };
        sceneFactory.Add(Primitive::CUBE, transform5, MaterialType::PHONG, phong);

        PointLight* pointLight = dynamic_cast<PointLight*>(GetLight(LightType::POINT));
        pointLight->position = { -2.0f, 2.0f, 2.0f };

        pointLight->ambient = { 0.05f, 0.05f, 0.05f };
        pointLight->diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight->specular = { 1.0f, 1.0f, 1.0f };

        pointLight->constant = 1.0f;
        pointLight->linear = 0.09f;
        pointLight->quadratic = 0.032f;

        sceneFactory.Add(LightType::POINT, pointLight);

        DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(GetLight(LightType::DIRECTION));
        directionalLight->direction = { 0.0f, -1.0f, 0.0f };

        directionalLight->ambient = { 0.05f, 0.05f, 0.05f };
        directionalLight->diffuse = { 0.4f, 0.4f, 0.4f };
        directionalLight->specular = { 0.5f, 0.5f, 0.5f };

        sceneFactory.Add(LightType::DIRECTION, directionalLight);

        SpotLight* spotLight = dynamic_cast<SpotLight*>(GetLight(LightType::SPOTLIGHT));

        spotLight->position = { 0.0f, 0.0f, 0.0f };
        spotLight->direction = { 0.0f, 0.0f, -1.0f };

        spotLight->ambient = { 0.0f, 0.0f, 0.0f };
        spotLight->diffuse = { 1.0f, 1.0f, 1.0f };
        spotLight->specular = { 1.0f, 1.0f, 1.0f };

        spotLight->constant = 1.0f;
        spotLight->linear = 0.09f;
        spotLight->quadratic = 0.032f;

        spotLight->cutOff = glm::cos(glm::radians(12.5f));
        spotLight->outerCutOff = glm::cos(glm::radians(15.0f));

        sceneFactory.Add(LightType::SPOTLIGHT, spotLight);

        return sceneFactory.GetScene();
    }
}