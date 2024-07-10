#include "SceneManager.h"
#include "SceneFactory.h"
#include <iostream>

#include <glm/ext/quaternion_trigonometric.hpp>

#include "Settings/App.h"

namespace Rutile {
    Scene SceneManager::GetScene(SceneType scene) {
        switch (scene) {
            default: {
                std::cout << "ERROR: Unknown SceneType" << std::endl;
                return GetTriangleScene();
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
            /*
            case SceneType::MULTI_SHADOW_CASTER_SHADOW_MAP_TESTING_SCENE: {
                return GetMultiLightShadowMapTestingScene();
            }
            case SceneType::OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE: {
                return GetOmnidirectionalShadowMapTestingScene();
            }
            case SceneType::GENERAL_SCENE: {
                return GetGeneralScene();
            }
            */
            case SceneType::DOUBLE_POINT_LIGHT_TEST_SCENE: {
                return GetDoublePointLightTestScene();
            }
            case SceneType::ALL_SPHERES: {
                return GetAllSpheresScene();
            }
        }
    }

    Phong SceneManager::GetPhong(const Solid& solid) {
        Phong phong;

        phong.diffuse = solid.color;
        phong.ambient = solid.color * 0.4f;
        phong.specular = phong.ambient * 0.2f;

        phong.shininess = 16.0f;

        return phong;
    }

    Solid SceneManager::GetSolid(const Phong& phong) {
        Solid solid;

        solid.color = phong.diffuse;

        return solid;
    }

    Scene SceneManager::GetTriangleScene() {
        SceneFactory sceneFactory{ };

        Transform transform{ };

        Solid solid{ };
        solid.color = { 1.0f, 0.0f, 0.0f };

        Phong phong = GetPhong(solid);

        sceneFactory.Add("Hello Triangle", Primitive::TRIANGLE, transform, "Triangle Material", solid, phong);

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetOriginalScene() {
        SceneFactory sceneFactory{ };

        Solid solid{ };
        solid.color = { 1.0f, 0.0f, 1.0f };

        Phong phong = GetPhong(solid);

        Solid solid2{ };
        solid2.color = { 0.2f, 0.5f, 0.7f };

        Phong phong2 = GetPhong(solid2);

        Phong phong3{ };

        phong3.ambient = { 1.0f, 0.5f, 0.31f };
        phong3.diffuse = { 1.0f, 0.5f, 0.31f };
        phong3.specular = { 0.5f, 0.5f, 0.5f };
        phong3.shininess = 32.0f;

        Solid solid3 = GetSolid(phong3);

        Solid solid4{ };
        solid4.color = { 1.0f, 1.0f, 0.0f };

        Phong phong4 = GetPhong(solid4);

        Transform transform1{ };
        transform1.position = { 1.0f, 1.0f, 0.0f };
        MaterialIndex mat1 = sceneFactory.Add("Top Right", Primitive::TRIANGLE, transform1, "Material 1", solid, phong);

        Transform transform2{ };
        transform2.position = { -1.0f, -1.0f, 0.0f };
        MaterialIndex mat2 = sceneFactory.Add("Bottom Left", Primitive::SQUARE, transform2, "Material 2", solid2, phong2);

        Transform transform3{ };
        transform3.position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add("Center", Primitive::CUBE, transform3, mat1);

        Transform transform4{ };
        transform4.position = { 1.0f, -1.0f, 0.0f };
        sceneFactory.Add("Bottom Right", Primitive::CUBE, transform4, mat2);

        Transform transform5{ };
        transform5.position = { -1.0f, 1.0f, 0.0f };
        MaterialIndex mat3 = sceneFactory.Add("Top Left", Primitive::CUBE, transform5, "Material 3", solid3, phong3);

        Transform transform6{ };
        transform6.scale = { 5.0f, 1.0f, 3.0f };
        transform6.position = { 0.0f, -2.0f, 0.0f };
        MaterialIndex mat4 = sceneFactory.Add("Floor", Primitive::CUBE, transform6, "Material 4", solid4, phong4);

        Transform transform7{ };
        transform7.scale = { 5.0f, 5.0f, 1.0f };
        transform7.position = { 0.0f, 0.0f, -2.0f };
        sceneFactory.Add("Floor", Primitive::CUBE, transform7, mat4);

        //PointLight pointLight{ };
        //pointLight.position = { 0.0f, 0.0f, 1.0f };

        //pointLight.ambient = { 0.05f, 0.05f, 0.05f };
        //pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
        //pointLight.specular = { 1.0f, 1.0f, 1.0f };

        //pointLight.constant = 1.0f;
        //pointLight.linear = 0.65f;
        //pointLight.quadratic = 0.032f;

        //sceneFactory.Add(pointLight);

        //PointLight pointLight2{ };
        //pointLight2.position = { -2.0f, 0.0f, 1.0f };

        //pointLight2.ambient = { 0.05f, 0.05f, 0.05f };
        //pointLight2.diffuse = { 0.8f, 0.8f, 0.8f };
        //pointLight2.specular = { 1.0f, 1.0f, 1.0f };

        //pointLight2.constant = 1.0f;
        //pointLight2.linear = 0.65f;
        //pointLight2.quadratic = 0.032f;

        //sceneFactory.Add(pointLight2);

        DirectionalLight dirLight{ };
        dirLight.ambient =  { 0.6f, 0.6f, 0.6f };
        dirLight.specular = { 0.6f, 0.6f, 0.6f };
        dirLight.diffuse =  { 0.6f, 0.6f, 0.6f };

        sceneFactory.Add(dirLight);

        Transform ball1{ };
        ball1.position = { 2.0f, 0.0f, 0.0f };
        ball1.scale = { 0.5f, 0.5f, 0.5f };

        sceneFactory.Add("Ball 1", Primitive::SPHERE, ball1, mat3);

        return sceneFactory.GetScene();
    }


    Scene SceneManager::GetShadowMapTestingScene() {
        SceneFactory sceneFactory{ };

        Phong phong1{ };
        phong1.diffuse = { 0.324f, 0.474f, 0.974f };
        phong1.ambient = { 0.275f, 0.64f, 0.234f };
        phong1.specular = { 0.432f, 0.8367f, 0.123f };
        phong1.shininess = 15.0f;

        Solid solid1 = GetSolid(phong1);

        Phong phong2{ };
        phong2.diffuse = { 0.84f, 0.753f, 0.859f };
        phong2.ambient = { 0.569f, 0.5638f, 0.194f };
        phong2.specular = { 0.113f, 0.754f, 0.943f };
        phong2.shininess = 64.0f;

        Solid solid2 = GetSolid(phong2);

        Phong phong3{ };
        phong3.diffuse = { 0.129f, 0.00f, 0.333f };
        phong3.ambient = { 0.783f, 0.356f, 0.324566f };
        phong3.specular = { 0.012f, 0.268f, 0.73f };
        phong3.shininess = 128.0f;

        Solid solid3 = GetSolid(phong3);

        Phong phong4{ };
        phong4.diffuse = { 0.129f, 0.00f, 0.333f };
        phong4.ambient = { 0.569f, 0.5638f, 0.194f };
        phong4.specular = { 0.432f, 0.8367f, 0.123f };
        phong4.shininess = 16.0f;

        Solid solid4 = GetSolid(phong4);

        DirectionalLight dirLight{ };
        dirLight.direction = { -1.0f, -1.0f, -1.0f };
        dirLight.diffuse = { 1.0f, 1.0f, 1.0f };
        dirLight.ambient = { 1.0f, 1.0f, 1.0f };
        dirLight.specular = { 1.0f, 1.0f, 1.0f };

        sceneFactory.Add(dirLight);

        Transform floorTransform{ };
        floorTransform.position.y = -1.0f;
        floorTransform.scale = { 30.0f, 1.0f, 30.0f };

        MaterialIndex mat1 = sceneFactory.Add("Floor", Primitive::CUBE, floorTransform, "Material 1", solid1, phong1);

        Transform box1{ };
        box1.position = { 3.0f, 0.0f, 3.0f };
        box1.scale = { 0.5f, 1.0f, 0.5f };
        box1.rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        MaterialIndex mat2 = sceneFactory.Add("Box 1", Primitive::CUBE, box1, "Material 2", solid2, phong2);

        Transform box2{ };
        box2.position = { 3.0f, 0.0f, -3.0f };
        box2.rotation = glm::angleAxis(glm::radians(30.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        MaterialIndex mat3 = sceneFactory.Add("Box 2", Primitive::CUBE, box2, "Material 3", solid3, phong3);

        Transform box3{ };
        box3.position = { 3.0f, 1.0f, -3.0f };
        box3.rotation = glm::angleAxis(glm::radians(60.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });

        sceneFactory.Add("Box 3", Primitive::CUBE, box3, "Material 4", solid4, phong4);

        Transform box4{ };
        box4.position.y = 2.0f;

        sceneFactory.Add("Box 4", Primitive::CUBE, box4, mat3);

        return sceneFactory.GetScene();
    }
    /*
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

    Scene SceneManager::GetOmnidirectionalShadowMapTestingScene() {
        SceneFactory sceneFactory{ };

        Solid* solid = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        solid->color = { 1.0f, 1.0f, 1.0f };

        Transform* lightTransform = GetTransform();
        lightTransform->position = { 0.0f, 3.0f, -10.0f };
        lightTransform->scale = { 0.4f, 0.4f, 0.4f };

        sceneFactory.Add(Primitive::CUBE, lightTransform, MaterialType::SOLID, solid);

        PointLight* pointLight = dynamic_cast<PointLight*>(GetLight(LightType::POINT));
        pointLight->diffuse = { 0.2f, 0.2f, 0.2f };
        pointLight->ambient = { 0.2f, 0.2f, 0.2f };
        pointLight->specular = { 0.2f, 0.2f, 0.2f };

        pointLight->constant = 0.62f;
        pointLight->linear = 0.175f;
        pointLight->quadratic = 0.035f;

        pointLight->position = lightTransform->position;

        sceneFactory.Add(LightType::POINT, pointLight);

        Phong* phong = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong->ambient = { 1.0f, 0.5f, 0.31f };
        phong->diffuse = { 1.0f, 0.5f, 0.31f };
        phong->specular = { 0.5f, 0.5f, 0.5f };
        phong->shininess = 32.0f;

        Transform* floor = GetTransform();
        floor->position = { 0.0f, -1.0f, -10.0f };
        floor->scale = { 30.0f, 1.0f, 60.0f };

        sceneFactory.Add(Primitive::CUBE, floor, MaterialType::PHONG, phong);

        Transform* roof = GetTransform();
        roof->position = { 0.0f, 10.0f, -10.0f };
        roof->scale = { 30.0f, 1.f, 60.0f };

        sceneFactory.Add(Primitive::CUBE, roof, MaterialType::PHONG, phong);

        Transform* posXWall = GetTransform();
        posXWall->position = { 10.0f, 0.0f, -10.0f };
        posXWall->scale = { 1.0f, 30.0f, 60.0f };

        sceneFactory.Add(Primitive::CUBE, posXWall, MaterialType::PHONG, phong);

        Transform* negXWall = GetTransform();
        negXWall->position = { -10.0f, 0.0f, -10.0f };
        negXWall->scale = { 1.0f, 30.0f, 60.0f };

        sceneFactory.Add(Primitive::CUBE, negXWall, MaterialType::PHONG, phong);

        Transform* negZWall = GetTransform();
        negZWall->position = { 0.0f, 0.0f, -20.0f };
        negZWall->scale = { 30.0f, 30.0f, 1.0f };

        sceneFactory.Add(Primitive::CUBE, negZWall, MaterialType::PHONG, phong);

        Transform* posZWall = GetTransform();
        posZWall->position = { 0.0f, 0.0f, 25.0f };
        posZWall->scale = { 30.0f, 30.0f, 1.0f };

        sceneFactory.Add(Primitive::CUBE, posZWall, MaterialType::PHONG, phong);

        Transform* cube1 = GetTransform();
        cube1->position = { -5.0f, 2.0f, -12.0f };
        cube1->scale = glm::vec3{ 1.2f };
        cube1->rotation = glm::angleAxis(glm::radians(25.0f), normalize(glm::vec3{ 1.0f, 1.0f, 1.0f }));

        sceneFactory.Add(Primitive::CUBE, cube1, MaterialType::PHONG, phong);

        Transform* cube2 = GetTransform();
        cube2->position = { 6.0f, 7.0f, -8.0f };
        cube2->rotation = glm::angleAxis(glm::radians(71.0f), normalize(glm::vec3{ 1.0f, -1.0f, 1.0f }));

        sceneFactory.Add(Primitive::CUBE, cube2, MaterialType::PHONG, phong);

        Transform* cube3 = GetTransform();
        cube3->position = { 0.0f, 5.0f, -14.0f };
        cube3->rotation = glm::angleAxis(glm::radians(45.0f), normalize(glm::vec3{ -1.0f, 1.0f, -1.0f }));

        sceneFactory.Add(Primitive::CUBE, cube3, MaterialType::PHONG, phong);

        Transform* cube4 = GetTransform();
        cube4->position = { 4.0f, 5.0f, -14.0f };
        cube4->scale = glm::vec3{ 3.0f };
        cube4->rotation = glm::angleAxis(glm::radians(63.5f), normalize(glm::vec3{ 1.0f, 3.0f, -5.0f }));

        sceneFactory.Add(Primitive::CUBE, cube4, MaterialType::PHONG, phong);

        Transform* cube5 = GetTransform();
        cube5->position = { 6.0f, 7.0f, -16.0f };
        cube5->scale = glm::vec3{ 0.5f };
        cube5->rotation = glm::angleAxis(glm::radians(15.5f), normalize(glm::vec3{ -3.0f, 1.0f, 2.5f }));

        sceneFactory.Add(Primitive::CUBE, cube5, MaterialType::PHONG, phong);

        float bonus = 0.0f;
        for (int i = 0; i < 10; ++i) {
            Transform* slit = GetTransform();

            float x = -4.0f + (float)i * (6.0f / 10.0f) + bonus;
            bonus += (float)i * (6.0f / 10.0f) / 9.0f;
            slit->position = { x, 7.5f, -10.0f };

            float scaleZ = 0.5f + (float)i * (1.5f / 10.0f) + bonus;
            slit->scale = { 0.3f, 0.05f, scaleZ };

            sceneFactory.Add(Primitive::CUBE, slit, MaterialType::PHONG, phong);
        }

        Transform* slit = GetTransform();
        slit->position = { -2.0f, 8.5f, -10.0f };
        slit->scale = { 3.0f, 0.05f, 0.5f };
        slit->rotation = glm::angleAxis(glm::radians(15.0f), normalize(glm::vec3{ 0.0f, 1.0f, 0.0f }));

        sceneFactory.Add(Primitive::CUBE, slit, MaterialType::PHONG, phong);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetGeneralScene() {
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

        Transform* lightTransform = GetTransform();
        lightTransform->position = { -4.0f, 1.5f, 0.0f };
        lightTransform->scale = { 0.2f, 0.2f, 0.2f };
        Solid* solidWhite = dynamic_cast<Solid*>(GetMaterial(MaterialType::SOLID));
        solidWhite->color = { 1.0f, 1.0f, 1.0f };

        sceneFactory.Add(Primitive::CUBE, lightTransform, MaterialType::SOLID, solidWhite);

        Phong* phong = dynamic_cast<Phong*>(GetMaterial(MaterialType::PHONG));
        phong->ambient = { 1.0f, 0.5f, 0.31f };
        phong->diffuse = { 1.0f, 0.5f, 0.31f };
        phong->specular = { 0.5f, 0.5f, 0.5f };
        phong->shininess = 32.0f;

        Transform* backWall = GetTransform();
        backWall->position = { -7.0f, 2.0f, 0.0f };
        backWall->scale = { 1.0f, 8.0f, 10.0f };

        sceneFactory.Add(Primitive::CUBE, backWall, MaterialType::PHONG, phong);

        PointLight* pointLight = dynamic_cast<PointLight*>(GetLight(LightType::POINT));
        pointLight->position = lightTransform->position;
        pointLight->diffuse =  { 0.6f, 0.6f, 0.6f };
        pointLight->ambient =  { 0.2f, 0.2f, 0.2f };
        pointLight->specular = { 0.1f, 0.1f, 0.1f };

        pointLight->constant = 0.62f;
        pointLight->linear = 0.175f;
        pointLight->quadratic = 0.035f;

        sceneFactory.Add(LightType::POINT, pointLight);

        return sceneFactory.GetScene();
    }
    */

    Scene SceneManager::GetDoublePointLightTestScene() {
        SceneFactory sceneFactory;

        Solid solid1{ };
        solid1.color = { 1.0f, 0.0f, 1.0f };

        Phong phong1 = GetPhong(solid1);

        Solid solid2{ };
        solid2.color = { 0.4f, 0.4f, 0.4f };

        Phong phong2 = GetPhong(solid2);

        Transform floor{ };
        floor.scale = { 11.0f, 1.0f, 11.0f };
        MaterialIndex mat2 = sceneFactory.Add("Floor", Primitive::CUBE, floor, "Material 2", solid2, phong2);

        Transform wall{ };
        wall.position = { 0.0f, 5.0f, -5.0f };
        wall.scale = { 11.0f, 11.0f, 1.0f };
        sceneFactory.Add("Wall", Primitive::CUBE, wall, mat2);

        Transform cube{ };
        cube.position.y = 2.0f;
        sceneFactory.Add("Cube", Primitive::CUBE, cube, "Material 1", solid1, phong1);

        PointLight pointLight{ };
        pointLight.position = { 3.0f, 4.0f, 0.0f };

        pointLight.ambient = { 0.05f, 0.05f, 0.05f };
        pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight.specular = { 1.0f, 1.0f, 1.0f };

        pointLight.constant = 1.0f;
        pointLight.linear = 0.0f;
        pointLight.quadratic = 0.032f;

        sceneFactory.Add(pointLight);

        PointLight pointLight2{ };
        pointLight2.position = { -3.0f, 4.0f, 0.0f };

        pointLight2.ambient = { 0.05f, 0.05f, 0.05f };
        pointLight2.diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight2.specular = { 1.0f, 1.0f, 1.0f };

        pointLight2.constant = 1.0f;
        pointLight2.linear = 0.0f;
        pointLight2.quadratic = 0.032f;

        sceneFactory.Add(pointLight2);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetAllSpheresScene() {
        SceneFactory sceneFactory;

        Solid solid1{ };
        solid1.color = { 1.0f, 0.0f, 0.0f };

        Phong phong1 = GetPhong(solid1);

        Transform ball1{ };
        MaterialIndex mat1 = sceneFactory.Add("Ball 1", Primitive::SPHERE, ball1, "Material 1", solid1, phong1);

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        return sceneFactory.GetScene();
    }
}