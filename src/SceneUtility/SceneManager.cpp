#include "SceneManager.h"
#include "SceneFactory.h"
#include <iostream>

#include <glm/ext/quaternion_trigonometric.hpp>

#include "Settings/App.h"

#include "utility/MaterialFactory.h"
#include "utility/GeometryFactory.h"
#include "utility/Random.h"

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
            case SceneType::OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE: {
                return GetOmnidirectionalShadowMapTestingScene();
            }
            case SceneType::DOUBLE_POINT_LIGHT_TEST_SCENE: {
                return GetDoublePointLightTestScene();
            }
            case SceneType::ALL_SPHERES: {
                return GetAllSpheresScene();
            }
            case SceneType::SPHERES_ON_SPHERES: {
                return GetSpheresOnSpheresScene();
            }
        }
    }

    Scene SceneManager::GetTriangleScene() {
        SceneFactory sceneFactory{ };

        Transform transform{ };
        sceneFactory.Add(GeometryFactory::Primitive::TRIANGLE, transform, MaterialFactory::Color::RED);

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetOriginalScene() {
        SceneFactory sceneFactory{ };

        Material mat1 = MaterialFactory::Construct({ 1.0f, 0.0f, 1.0f });
        Material mat2 = MaterialFactory::Construct({ 0.2f, 0.5f, 0.7f });

        Material::Phong phong3{ };

        phong3.ambient = { 1.0f, 0.5f, 0.31f };
        phong3.diffuse = { 1.0f, 0.5f, 0.31f };
        phong3.specular = { 0.5f, 0.5f, 0.5f };
        phong3.shininess = 32.0f;

        Material mat3 = MaterialFactory::Construct(phong3);

        Material mat4 = MaterialFactory::Construct({ 1.0f, 1.0f, 0.0f });

        Transform transform1{ };
        transform1.position = { 1.0f, 1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::TRIANGLE, transform1, mat1, "Top Right");

        Transform transform2{ };
        transform2.position = { -1.0f, -1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SQUARE, transform2, mat2, "Bottom Left");

        Transform transform3{ };
        transform3.position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, transform3, mat1, "Center");

        Transform transform4{ };
        transform4.position = { 1.0f, -1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, transform4, mat2, "Bottom Right");

        Transform transform5{ };
        transform5.position = { -1.0f, 1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, transform5, mat3, "Top Left");

        Transform transform6{ };
        transform6.scale = { 5.0f, 1.0f, 3.0f };
        transform6.position = { 0.0f, -2.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, transform6, mat4, "Floor");

        Transform transform7{ };
        transform7.scale = { 5.0f, 5.0f, 1.0f };
        transform7.position = { 0.0f, 0.0f, -2.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, transform7, mat4, "Floor");

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

        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball1, mat3, "Ball 1");

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetShadowMapTestingScene() {
        SceneFactory sceneFactory{};

        Material::Phong phong1{};
        phong1.diffuse = { 0.324f, 0.474f, 0.974f };
        phong1.ambient = { 0.275f, 0.64f, 0.234f };
        phong1.specular = { 0.432f, 0.8367f, 0.123f };
        phong1.shininess = 15.0f;
        Material mat1 = MaterialFactory::Construct(phong1);

        Material::Phong phong2{};
        phong2.diffuse = { 0.84f, 0.753f, 0.859f };
        phong2.ambient = { 0.569f, 0.5638f, 0.194f };
        phong2.specular = { 0.113f, 0.754f, 0.943f };
        phong2.shininess = 64.0f;
        Material mat2 = MaterialFactory::Construct(phong2);

        Material::Phong phong3{};
        phong3.diffuse = { 0.129f, 0.00f, 0.333f };
        phong3.ambient = { 0.783f, 0.356f, 0.324566f };
        phong3.specular = { 0.012f, 0.268f, 0.73f };
        phong3.shininess = 128.0f;
        Material mat3 = MaterialFactory::Construct(phong3);

        Material::Phong phong4{};
        phong4.diffuse = { 0.129f, 0.00f, 0.333f };
        phong4.ambient = { 0.569f, 0.5638f, 0.194f };
        phong4.specular = { 0.432f, 0.8367f, 0.123f };
        phong4.shininess = 16.0f;
        Material mat4 = MaterialFactory::Construct(phong4);

        DirectionalLight dirLight{};
        dirLight.direction = { -1.0f, -1.0f, -1.0f };
        dirLight.diffuse = { 1.0f, 1.0f, 1.0f };
        dirLight.ambient = { 1.0f, 1.0f, 1.0f };
        dirLight.specular = { 1.0f, 1.0f, 1.0f };
        sceneFactory.Add(dirLight);

        Transform floorTransform{};
        floorTransform.position.y = -1.0f;
        floorTransform.scale = { 30.0f, 1.0f, 30.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, floorTransform, mat1, "Floor");

        Transform box1{};
        box1.position = { 3.0f, 0.0f, 3.0f };
        box1.scale = { 0.5f, 1.0f, 0.5f };
        box1.rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, box1, mat2, "Box 1");

        Transform box2{};
        box2.position = { 3.0f, 0.0f, -3.0f };
        box2.rotation = glm::angleAxis(glm::radians(30.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, box2, mat3, "Box 2");

        Transform box3{};
        box3.position = { 3.0f, 1.0f, -3.0f };
        box3.rotation = glm::angleAxis(glm::radians(60.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, box3, mat4, "Box 3");

        Transform box4{};
        box4.position.y = 2.0f;
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, box4, mat3, "Box 4");

        return sceneFactory.GetScene();
    }
    
    Scene SceneManager::GetOmnidirectionalShadowMapTestingScene() {
        SceneFactory sceneFactory{};

        Material mat1 = MaterialFactory::Construct({ 1.0f, 1.0f, 1.0f });

        Transform lightTransform{};
        lightTransform.position = { 0.0f, 3.0f, -10.0f };
        lightTransform.scale = { 0.4f, 0.4f, 0.4f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, lightTransform, mat1, "Light");

        PointLight pointLight{};
        pointLight.diffuse = { 0.5f, 0.5f, 0.5f };
        pointLight.ambient = { 0.5f, 0.5f, 0.5f };
        pointLight.specular = { 0.5f, 0.5f, 0.5f };
        pointLight.constant = 0.62f;
        pointLight.linear = 0.175f;
        pointLight.quadratic = 0.035f;
        pointLight.position = lightTransform.position;
        sceneFactory.Add(pointLight);

        Material::Phong phong2{};
        phong2.ambient = { 1.0f, 0.5f, 0.31f };
        phong2.diffuse = { 1.0f, 0.5f, 0.31f };
        phong2.specular = { 0.5f, 0.5f, 0.5f };
        phong2.shininess = 32.0f;
        Material mat2 = MaterialFactory::Construct(phong2);

        Transform floor{};
        floor.position = { 0.0f, -1.0f, -10.0f };
        floor.scale = { 30.0f, 1.0f, 60.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, floor, mat2, "Floor");

        Transform roof{};
        roof.position = { 0.0f, 10.0f, -10.0f };
        roof.scale = { 30.0f, 1.0f, 60.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, roof, mat2, "Roof");

        Transform posXWall{};
        posXWall.position = { 10.0f, 0.0f, -10.0f };
        posXWall.scale = { 1.0f, 30.0f, 60.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, posXWall, mat2, "Positive X Wall");

        Transform negXWall{};
        negXWall.position = { -10.0f, 0.0f, -10.0f };
        negXWall.scale = { 1.0f, 30.0f, 60.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, negXWall, mat2, "Negative X Wall");

        Transform negZWall{};
        negZWall.position = { 0.0f, 0.0f, -20.0f };
        negZWall.scale = { 30.0f, 30.0f, 1.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, negZWall, mat2, "Negative Z Wall");

        Transform posZWall{};
        posZWall.position = { 0.0f, 0.0f, 25.0f };
        posZWall.scale = { 30.0f, 30.0f, 1.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, posZWall, mat2, "Positive Z Wall");

        Transform cube1{};
        cube1.position = { -5.0f, 2.0f, -12.0f };
        cube1.scale = glm::vec3{ 1.2f };
        cube1.rotation = glm::angleAxis(glm::radians(25.0f), normalize(glm::vec3{ 1.0f, 1.0f, 1.0f }));
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube1, mat2, "Box 1");

        Transform cube2{};
        cube2.position = { 6.0f, 7.0f, -8.0f };
        cube2.rotation = glm::angleAxis(glm::radians(71.0f), normalize(glm::vec3{ 1.0f, -1.0f, 1.0f }));
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube2, mat2, "Box 2");

        Transform cube3{};
        cube3.position = { 0.0f, 5.0f, 0.0f };
        cube3.scale = { 2.0f, 1.0f, 1.0f };
        cube3.rotation = glm::angleAxis(glm::radians(45.0f), normalize(glm::vec3{ 1.0f, 1.0f, 0.0f }));
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube3, mat2, "Box 3");

        Transform cube4{};
        cube4.position = { 3.0f, 0.0f, 12.0f };
        cube4.rotation = glm::angleAxis(glm::radians(60.0f), normalize(glm::vec3{ -1.0f, -1.0f, 1.0f }));
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube4, mat2, "Box 4");

        Transform cube5{};
        cube5.position = { -8.0f, 0.0f, 13.0f };
        cube5.scale = { 1.0f, 2.0f, 1.0f };
        cube5.rotation = glm::angleAxis(glm::radians(95.0f), normalize(glm::vec3{ 1.0f, 1.0f, 1.0f }));
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube5, mat2, "Box 5");

        float bonus = 0.0f;
        for (int i = 0; i < 10; ++i) {
            Transform slit{ };

            float x = -4.0f + (float)i * (6.0f / 10.0f) + bonus;
            bonus += (float)i * (6.0f / 10.0f) / 9.0f;
            slit.position = { x, 7.5f, -10.0f };

            float scaleZ = 0.5f + (float)i * (1.5f / 10.0f) + bonus;
            slit.scale = { 0.3f, 0.05f, scaleZ };

            sceneFactory.Add(GeometryFactory::Primitive::CUBE, slit, mat2, "Slit " + std::to_string(i + 1));
        }

        Transform slit{ };
        slit.position = { -2.0f, 8.5f, -10.0f };
        slit.scale = { 3.0f, 0.05f, 0.5f };
        slit.rotation = glm::angleAxis(glm::radians(15.0f), normalize(glm::vec3{ 0.0f, 1.0f, 0.0f }));

        sceneFactory.Add(GeometryFactory::Primitive::CUBE, slit, mat2, "Board");

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetDoublePointLightTestScene() {
        SceneFactory sceneFactory;

        Material mat1 = MaterialFactory::Construct(MaterialFactory::Color::PINK);
        Material mat2 = MaterialFactory::Construct({ 0.4f, 0.4f, 0.4f });

        Transform floor{ };
        floor.scale = { 11.0f, 1.0f, 11.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, floor, mat2, "Floor");

        Transform wall{ };
        wall.position = { 0.0f, 5.0f, -5.0f };
        wall.scale = { 11.0f, 11.0f, 1.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, wall, mat2, "Wall");

        Transform cube{ };
        cube.position.y = 2.0f;
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, cube, mat1, "Cube");

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

        Material mat1 = MaterialFactory::Construct({ 1.0f, 0.0f, 0.0f });
        Material mat2 = MaterialFactory::Construct({ 0.0f, 1.0f, 0.0f });

        Transform ball1{ };
        ball1.position = { 0.0f, 0.0f, -1.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball1, mat1, "Ball 1");

        Transform ball2{ };
        ball2.position = { 0.0f, -251.0f, -1.0f };
        ball2.scale = { 250.0f, 250.0f, 250.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball2, mat2, "Ball 2");

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetSpheresOnSpheresScene() {
        SceneFactory sceneFactory;

        Material mat1 = MaterialFactory::Construct({ 245.0f / 255.0f, 66.0f / 255.0f, 203.0f / 255.0f }, "Main Ball Material");

        Transform mainBall{ };
        mainBall.scale = { 3.0f, 3.0f, 3.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, mainBall, mat1, "Main Ball");

        for (int i = 0; i < 99; ++i) {
            Material mat = MaterialFactory::Construct(RandomVec3());

            Transform transform{ };
            transform.position = RandomUnitVec3() * 4.5f;
            float radius = RandomFloat(0.7f, 1.3f);
            transform.scale = { radius, radius, radius };
            sceneFactory.Add(GeometryFactory::Primitive::SPHERE, transform, mat, "Ball " + std::to_string(i));
        }

        return sceneFactory.GetScene();
    }
}