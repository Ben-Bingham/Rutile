#include "SceneManager.h"
#include "SceneFactory.h"
#include <iostream>

#include <glm/ext/quaternion_trigonometric.hpp>

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

        if (m_HaveCreatedDirectionalLight) {
            delete m_DirectionalLight;
            m_HaveCreatedDirectionalLight = false;
        }

        switch (scene) {
            default: {
                std::cout << "ERROR: Unknown SceneType" << std::endl;
                break;
            }
            case SceneType::TRIANGLE_SCENE: {
                return GetTriangleScene();
            }
            case SceneType::ORIGINAL_SCENE: {
                return GetOriginalScene();
            }
            case SceneType::SHADOW_MAP_TESTING_SCENE: {
                return GetShadowMapTestingScene();
            }
            case SceneType::MULTI_SHADOW_CASTER_SHADOW_MAP_TESTING_SCENE: {
                return GetMultiLightShadowMapTestingScene();
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
            case LightType::SPOTLIGHT: {
                m_Lights.push_back(new SpotLight{ });
                m_LightTypes.push_back(LightType::SPOTLIGHT);
                break;
            }
        }

        return m_Lights.back();
    }

    DirectionalLight* SceneManager::GetDirectionalLight() {
        m_DirectionalLight = new DirectionalLight;
        m_HaveCreatedDirectionalLight = true;
        return m_DirectionalLight;
    }

    Transform* SceneManager::GetTransform() {
        m_Transforms.push_back(new Transform{ });

        return m_Transforms.back();
    }

    Scene SceneManager::GetTriangleScene() {
        SceneFactory sceneFactory{ };

        Transform* transform = GetTransform();
        Solid* solid = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        solid->color = { 1.0f, 0.0f, 0.0f };
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

        DirectionalLight* directionalLight = GetDirectionalLight();
        directionalLight->direction = { 0.0f, -1.0f, 0.0f };

        directionalLight->ambient = { 0.05f, 0.05f, 0.05f };
        directionalLight->diffuse = { 0.4f, 0.4f, 0.4f };
        directionalLight->specular = { 0.5f, 0.5f, 0.5f };

        sceneFactory.Add(directionalLight);

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

    Scene SceneManager::GetShadowMapTestingScene() {
        SceneFactory sceneFactory{ };

        Phong* phong1 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong1->diffuse = { 0.324f, 0.474f, 0.974f };
        phong1->ambient = { 0.275f, 0.64f, 0.234f };
        phong1->specular = { 0.432f, 0.8367f, 0.123f };
        phong1->shininess = 15.0f;

        Phong* phong2 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong2->diffuse = { 0.84f, 0.753f, 0.859f };
        phong2->ambient = { 0.569f, 0.5638f, 0.194f };
        phong2->specular = { 0.113f, 0.754f, 0.943f };
        phong2->shininess = 64.0f;

        Phong* phong3 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong3->diffuse = { 0.129f, 0.00f, 0.333f };
        phong3->ambient = { 0.783f, 0.356f, 0.324566f };
        phong3->specular = { 0.012f, 0.268f, 0.73f };
        phong3->shininess = 128.0f;

        Phong* phong4 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong4->diffuse = { 0.129f, 0.00f, 0.333f };
        phong4->ambient = { 0.569f, 0.5638f, 0.194f };
        phong4->specular = { 0.432f, 0.8367f, 0.123f };
        phong4->shininess = 16.0f;

        DirectionalLight* dirLight1 = GetDirectionalLight();
        dirLight1->direction = { -1.0f, -1.0f, -1.0f };
        dirLight1->diffuse = { 1.0f, 1.0f, 1.0f };
        dirLight1->ambient = { 1.0f, 1.0f, 1.0f };
        dirLight1->specular = { 1.0f, 1.0f, 1.0f };

        sceneFactory.Add(dirLight1);

        Transform* floorTransform = GetTransform();
        floorTransform->position.y = -1.0f;
        floorTransform->scale = { 30.0f, 1.0f, 30.0f };

        sceneFactory.Add(Primitive::CUBE, floorTransform, MaterialType::PHONG, phong1);

        Transform* box1 = GetTransform();
        box1->position = { 3.0f, 0.0f, 3.0f };
        box1->scale = { 0.5f, 1.0f, 0.5f };
        box1->rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box1, MaterialType::PHONG, phong2);

        Transform* box2 = GetTransform();
        box2->position = { 3.0f, 0.0f, -3.0f };
        box2->rotation = glm::angleAxis(glm::radians(30.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box2, MaterialType::PHONG, phong3);

        Transform* box3 = GetTransform();
        box3->position = { 3.0f, 1.0f, -3.0f };
        box3->rotation = glm::angleAxis(glm::radians(60.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box3, MaterialType::PHONG, phong4);

        Transform* box4 = GetTransform();
        box4->position.y = 2.0f;

        sceneFactory.Add(Primitive::CUBE, box4, MaterialType::PHONG, phong3);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetMultiLightShadowMapTestingScene() {
        SceneFactory sceneFactory{ };

        Phong* phong1 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong1->diffuse = { 0.324f, 0.474f, 0.974f };
        phong1->ambient = { 0.275f, 0.64f, 0.234f };
        phong1->specular = { 0.432f, 0.8367f, 0.123f };
        phong1->shininess = 15.0f;

        Phong* phong2 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong2->diffuse = { 0.84f, 0.753f, 0.859f };
        phong2->ambient = { 0.569f, 0.5638f, 0.194f };
        phong2->specular = { 0.113f, 0.754f, 0.943f };
        phong2->shininess = 64.0f;

        Phong* phong3 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong3->diffuse = { 0.129f, 0.00f, 0.333f };
        phong3->ambient = { 0.783f, 0.356f, 0.324566f };
        phong3->specular = { 0.012f, 0.268f, 0.73f };
        phong3->shininess = 128.0f;

        Phong* phong4 = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong4->diffuse = { 0.129f, 0.00f, 0.333f };
        phong4->ambient = { 0.569f, 0.5638f, 0.194f };
        phong4->specular = { 0.432f, 0.8367f, 0.123f };
        phong4->shininess = 16.0f;

        DirectionalLight* dirLight1 = GetDirectionalLight();
        dirLight1->direction = { -1.0f, -1.0f, -1.0f };
        dirLight1->diffuse = { 1.0f, 1.0f, 1.0f };
        dirLight1->ambient = { 1.0f, 1.0f, 1.0f };
        dirLight1->specular = { 1.0f, 1.0f, 1.0f };

        sceneFactory.Add(dirLight1);

        Transform* floorTransform = GetTransform();
        floorTransform->position.y = -1.0f;
        floorTransform->scale = { 30.0f, 1.0f, 90.0f };

        sceneFactory.Add(Primitive::CUBE, floorTransform, MaterialType::PHONG, phong1);

        Transform* box1 = GetTransform();
        box1->position = { 3.0f, 0.0f, 3.0f };
        box1->scale = { 0.5f, 1.0f, 0.5f };
        box1->rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box1, MaterialType::PHONG, phong2);

        Transform* box2 = GetTransform();
        box2->position = { 3.0f, 0.0f, -3.0f };
        box2->rotation = glm::angleAxis(glm::radians(30.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box2, MaterialType::PHONG, phong3);

        Transform* box3 = GetTransform();
        box3->position = { 3.0f, 1.0f, -3.0f };
        box3->rotation = glm::angleAxis(glm::radians(60.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add(Primitive::CUBE, box3, MaterialType::PHONG, phong2);

        Transform* box4 = GetTransform();
        box4->position.y = 2.0f;

        sceneFactory.Add(Primitive::CUBE, box4, MaterialType::PHONG, phong3);

        Transform* box5 = GetTransform();
        box5->position = { 0.0f, 0.0f, 15.0f };

        sceneFactory.Add(Primitive::CUBE, box5, MaterialType::PHONG, phong2);

        Transform* box6 = GetTransform();
        box6->position = { 0.0f, 0.0f, -15.0f };

        sceneFactory.Add(Primitive::CUBE, box6, MaterialType::PHONG, phong3);

        return sceneFactory.GetScene();
    }
}