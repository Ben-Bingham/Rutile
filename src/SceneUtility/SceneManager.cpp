#include "SceneManager.h"
#include "SceneFactory.h"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Settings/App.h"

#include "Utility/MaterialFactory.h"
#include "Utility/GeometryFactory.h"
#include "Utility/Random.h"
#include "Utility/RayTracing/AABBFactory.h"
#include "Utility/RayTracing/BoundingVolumeHierarchy/BVHFactory.h"

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
            case SceneType::HOLLOW_GLASS_SPHERE: {
                return GetHollowGlassSphereScene();
            }
            case SceneType::RAY_TRACING_IN_ONE_WEEKEND: {
                return GetRayTracingInOneWeekendScene();
            }
            case SceneType::CORNELL_BOX: {
                return GetCornellBoxScene();
            }
            case SceneType::BACKPACK: {
                return GetBackpackScene();
            }
            case SceneType::BVH_VISUALIZATION: {
                return GetBVHVisualizationScene();
            }
        }
    }

    Scene SceneManager::GetTriangleScene() {
        SceneFactory sceneFactory{ };

        sceneFactory.Add(GeometryFactory::Primitive::TRIANGLE, Transform{ }, MaterialFactory::Color::RED);

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
        mat1.type = Material::Type::EMISSIVE;

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

        Material mirrorMat = MaterialFactory::Construct({ 0.8f, 0.8f, 0.8f });
        mirrorMat.type = Material::Type::MIRROR;

        Material metal = MaterialFactory::Construct(MaterialFactory::Color::YELLOW);
        metal.fuzz = 1.0f;
        metal.type = Material::Type::MIRROR;

        Material dielectric = MaterialFactory::Construct(MaterialFactory::Color::BLUE);
        dielectric.type = Material::Type::DIELECTRIC;
        dielectric.indexOfRefraction = 1.5f;

        Material floorMat = MaterialFactory::Construct({ 0.0f, 1.0f, 0.0f });

        Material lightMat = MaterialFactory::Construct(MaterialFactory::Color::WHITE);
        lightMat.type = Material::Type::EMISSIVE;

        Transform ball1{ };
        ball1.position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball1, mat1, "Diffuse Ball");

        Transform ball2{ };
        ball2.position = { 0.0f, 0.0f, -2.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball2, mirrorMat, "Mirror Ball 1");

        Transform ball3{ };
        ball3.position = { 2.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball3, metal, "Mirror Ball 2");

        Transform ball4{ };
        ball4.position = { -2.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball4, dielectric, "Dielectric Ball");

        Transform floor{ };
        floor.position = { 0.0f, -251.0f, -1.0f };
        floor.scale = { 250.0f, 250.0f, 250.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, floor, floorMat, "Floor");

        Transform lightTransform{ };
        lightTransform.position = { 0.0f, 3.0f, 0.0f };
        lightTransform.scale = { 2.0f, 0.2f, 2.0f };
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, lightTransform, lightMat, "Light");

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

    Scene SceneManager::GetHollowGlassSphereScene() {
        SceneFactory sceneFactory{ };

        Material mat1 = MaterialFactory::Construct(MaterialFactory::Color::RED);
        Material floorMat = MaterialFactory::Construct(MaterialFactory::Color::GREEN);

        Material glass = MaterialFactory::Construct(MaterialFactory::Color::WHITE);
        glass.type = Material::Type::DIELECTRIC;
        glass.indexOfRefraction = 1.5f;

        Material airBubble = MaterialFactory::Construct(MaterialFactory::Color::WHITE);
        airBubble.type = Material::Type::DIELECTRIC;
        airBubble.indexOfRefraction = 1.0f / 1.5f;

        Transform ball1{ };
        ball1.position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball1, mat1, "Diffuse Ball");

        Transform glassTransform{ };
        glassTransform.position = { -2.0f, 0.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, glassTransform, glass, "Glass Ball");

        Transform airBubbleTransform{ };
        airBubbleTransform.position = { -2.0f, 0.0f, 0.0f };
        airBubbleTransform.scale = { 0.8f, 0.8f, 0.8f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, airBubbleTransform, airBubble, "Air Bubble");

        Transform floor{ };
        floor.position = { 0.0f, -251.0f, -1.0f };
        floor.scale = { 250.0f, 250.0f, 250.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, floor, floorMat, "Floor");

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetRayTracingInOneWeekendScene() {
        SceneFactory sceneFactory{ };

        Material groundMaterial = MaterialFactory::Construct(MaterialFactory::Color::GRAY);
        groundMaterial.type = Material::Type::DIFFUSE;

        Transform floorTransform{ };
        floorTransform.position = { 0.0f, -1000.0f, 0.0f };
        floorTransform.scale = { 1000.0f, 1000.0f, 1000.0f };

        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, floorTransform, groundMaterial, "Ground");

        int i = 0;
        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                Transform sphereTransform{ };
                sphereTransform.position = { a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat() };
                sphereTransform.scale = { 0.2f, 0.2f, 0.2f };

                float chosenMaterial = RandomFloat();
                if ((sphereTransform.position - glm::vec3{ 4.0f, 0.2f, 0.0f }).length() > 0.9f) {
                    Material sphereMaterial{ };

                    if (chosenMaterial < 0.8) {
                        // diffuse
                        glm::vec3 color = RandomVec3() * RandomVec3();
                        sphereMaterial = MaterialFactory::Construct(color);
                        sphereMaterial.type = Material::Type::DIFFUSE;
                    }
                    else if (chosenMaterial < 0.95) {
                        // metal
                        glm::vec3 albedo = RandomVec3(0.5f, 1.0f);
                        float fuzz = RandomFloat(0.0f, 0.5f);
                        sphereMaterial = MaterialFactory::Construct(albedo);
                        sphereMaterial.type = Material::Type::MIRROR;
                        sphereMaterial.fuzz = fuzz;
                    }
                    else {
                        // glass
                        sphereMaterial = MaterialFactory::Construct(MaterialFactory::Color::WHITE);
                        sphereMaterial.type = Material::Type::DIELECTRIC;
                        sphereMaterial.indexOfRefraction = 1.5f;
                    }

                    sceneFactory.Add(GeometryFactory::Primitive::SPHERE, sphereTransform, sphereMaterial, "Tiny Sphere #" + std::to_string(i));
                }
                ++i;
            }
        }

        Material mat1 = MaterialFactory::Construct(MaterialFactory::Color::WHITE);
        mat1.type = Material::Type::DIELECTRIC;
        mat1.indexOfRefraction = 1.5f;

        Transform ball1{ };
        ball1.position = { 0.0f, 1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball1, mat1, "Ball 1");

        Material mat2 = MaterialFactory::Construct({ 0.4f, 0.2f, 0.1f });
        mat2.type = Material::Type::DIFFUSE;

        Transform ball2{ };
        ball2.position = { -4.0f, 1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball2, mat2, "Ball 2");

        Material mat3 = MaterialFactory::Construct({ 0.7f, 0.6f, 0.5f });
        mat3.type = Material::Type::MIRROR;
        mat3.fuzz = 0.0f;

        Transform ball3{ };
        ball3.position = { 4.0f, 1.0f, 0.0f };
        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, ball3, mat3, "Ball 3");

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetCornellBoxScene() {
        SceneFactory sceneFactory;

        Material red   = MaterialFactory::Construct({ 0.65f, 0.05f, 0.05f });
        red.phong.shininess = 0.0f;

        Material white = MaterialFactory::Construct({ 0.73f, 0.73f, 0.73f });
        white.phong.shininess = 0.0f;

        Material green = MaterialFactory::Construct({ 0.12f, 0.45f, 0.15f });
        green.phong.shininess = 0.0f;

        Material light = MaterialFactory::Construct(glm::vec3{ 100.0f });
        light.phong.shininess = 0.0f;
        light.type = Material::Type::EMISSIVE;

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 0.0f,  0.0f,  0.0f },
            glm::vec3{ 0.0f,  5.55f, 0.0f },
            glm::vec3{ 5.55f, 5.55f, 0.0f },
            glm::vec3{ 5.55,  0.0f,  0.0f }
        ), Transform{}, white, "Back Wall");

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 0.0f,  0.0f, 0.0f  },
            glm::vec3{ 5.55f, 0.0f, 0.0f  },
            glm::vec3{ 5.55f, 0.0f, 5.55f },
            glm::vec3{ 0.0f,  0.0f, 5.55f }
        ), Transform{}, white, "Floor");

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 0.0f,  5.55f, 0.0f  },
            glm::vec3{ 0.0f,  5.55f, 5.55f },
            glm::vec3{ 5.55f, 5.55f, 5.55f },
            glm::vec3{ 5.55f, 5.55f, 0.0f  }
        ), Transform{}, white, "Roof");

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 5.55f, 0.0f,  0.0f  },
            glm::vec3{ 5.55f, 5.55f, 0.0f  },
            glm::vec3{ 5.55f, 5.55f, 5.55f },
            glm::vec3{ 5.55f, 0.0f,  5.55f }
        ), Transform{}, red, "Right Wall");

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 0.0f, 0.0f,  0.0f  },
            glm::vec3{ 0.0f, 0.0f,  5.55f },
            glm::vec3{ 0.0f, 5.55f, 5.55f },
            glm::vec3{ 0.0f, 5.55f, 0.0f  }
        ), Transform{}, green, "Left Wall");

        sceneFactory.Add(GeometryFactory::ConstructQuad(
            glm::vec3{ 2.13f, 5.549f, 2.27f },
            glm::vec3{ 2.13f, 5.549f, 3.32f },
            glm::vec3{ 3.43f, 5.549f, 3.32f },
            glm::vec3{ 3.43f, 5.549f, 2.27f }
        ), Transform{}, light, "Light");

        Transform smallBoxTransform{ };
        smallBoxTransform.position = { 2.65f + 0.825f, 0.825f, 2.95f + 0.825f };
        smallBoxTransform.scale = { 1.65f, 1.65f, 1.65f };
        smallBoxTransform.rotation = glm::angleAxis(-0.314f, glm::vec3{ 0.0f, 1.0f, 0.0f });
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, smallBoxTransform, white, "Small Box");

        Transform bigBoxTransform{ };
        bigBoxTransform.position = { 1.3f + 0.825f, 1.65f, 0.65f + 0.825f };
        bigBoxTransform.scale = { 1.65f, 3.3f, 1.65f };
        bigBoxTransform.rotation = glm::angleAxis(0.3925f, glm::vec3{ 0.0f, 1.0f, 0.0f });
        sceneFactory.Add(GeometryFactory::Primitive::CUBE, bigBoxTransform, white, "Big Box");

        App::camera.position = { 2.78f, 2.78f, 13.5f };
        App::settings.fieldOfView = 40.0f;

        PointLight pointLight;
        pointLight.position = { (3.43f + 2.13f) / 2.0f, 5.549f, (3.32f + 3.27f) / 2.0f };
        sceneFactory.Add(pointLight);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetBackpackScene() {
        SceneFactory sceneFactory;

        sceneFactory.Add("assets\\models\\backpack\\backpack.obj", Transform{ });

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        return sceneFactory.GetScene();
    }

    Scene SceneManager::GetBVHVisualizationScene() {
        SceneFactory sceneFactory{ };

        sceneFactory.Add(GeometryFactory::Primitive::SPHERE, Transform{ }, MaterialFactory::Color::RED);

        DirectionalLight dirLight{ };
        sceneFactory.Add(dirLight);

        int k = 0;
        for (auto object : sceneFactory.GetScene().objects) {
            BVHFactory::ReturnStructure2 structure2 = BVHFactory::Construct(sceneFactory.GetScene().geometryBank[object.geometry]);

            for (int i = 0; i < structure2.bank.Size(); ++i) {

                float y = 2.0f;

                if (structure2.bank[i].triangleCount <= 0) {
                    y = 4.0f;
                }

                sceneFactory.Add(GeometryFactory::Construct(structure2.bank[i].bbox), Transform{ { 2.0f * i + 2.0f, y + 4.0 * k, 0.0f } }, MaterialFactory::Construct(RandomVec3()));

                int offset = structure2.bank[i].triangleOffset;
                int count = structure2.bank[i].triangleCount;

                if (structure2.bank[i].triangleCount <= 0) {
                    continue;
                }

                std::vector<Triangle> tris;
                for (int i = offset; i < offset + count; ++i) {
                    tris.push_back(structure2.triangles[i]);
                }

                std::vector<Vertex> verts{ };

                for (auto tri : tris) {
                    Vertex v1 = { tri[0] };
                    Vertex v2 = { tri[1] };
                    Vertex v3 = { tri[2] };

                    verts.push_back(v1);
                    verts.push_back(v2);
                    verts.push_back(v3);
                }

                std::vector<Index> indices{ };

                int j = 0;
                for (auto vert : verts) {
                    indices.push_back(j);
                    ++j;
                }
                Geometry geo;

                geo.vertices = verts;
                geo.indices = indices;

                sceneFactory.Add(geo, Transform{ { 2.0f * i + 2.0f, 0.0f + 4.0 * k, 0.0f } }, MaterialFactory::Construct(RandomVec3()));
            }
            ++k;
        }

        return sceneFactory.GetScene();
    }
}