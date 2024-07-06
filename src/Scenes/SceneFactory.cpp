#include "SceneFactory.h"

#include "Settings/App.h"

namespace Rutile {
    Scene SceneFactory::GetScene() {
        return m_CurrentScene;
    }

    MaterialIndex SceneFactory::Add(const std::string& objectName, Primitive primitive, const Transform& transform, const std::string& materialName, const Solid& solid, const Phong& phong) {
        Object obj;

        auto geo = GetGeometry(primitive);

        obj.name = objectName;
        obj.geometry = App::geometryBank.Add(geo.second, geo.first);
        obj.transform = App::transformBank.Add(transform);
        obj.material = App::materialBank.Add(materialName, solid, phong);

        m_CurrentScene.objects.push_back(obj);

        return obj.material;
    }

    MaterialIndex SceneFactory::Add(const std::string& objectName, Primitive primitive, const Transform& transform, MaterialIndex material) {
        Object obj;

        auto geo = GetGeometry(primitive);

        obj.name = objectName;
        obj.geometry = App::geometryBank.Add(geo.second, geo.first);
        obj.transform = App::transformBank.Add(transform);
        obj.material = material;

        m_CurrentScene.objects.push_back(obj);

        return obj.material;
    }

    void SceneFactory::Add(const PointLight& pointLight) {
        m_CurrentScene.pointLights.push_back(pointLight);
    }

    void SceneFactory::Add(const DirectionalLight& light) {
        m_CurrentScene.directionalLight = light;
        m_CurrentScene.m_EnableDirectionalLight = true;
    }

    std::pair<Geometry, std::string> SceneFactory::GetGeometry(Primitive primitive) {
        Geometry geo;
        std::string geometryName;
        switch (primitive) {
        case Primitive::TRIANGLE:
            geometryName = "Triangle";

            geo.vertices = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.0f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.5f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            geo.indices = {
                2, 1, 0
            };


            break;

        case Primitive::CUBE:
            geometryName = "Cube";

            geo.vertices = {
                //      Position                         Normal                             Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  0.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f } },

                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  0.0f,  1.0f }, glm::vec2{ 0.0f, 1.0f } },

                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{ -1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  1.0f,  0.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, -0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{  0.0f, -1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },

                Vertex{ glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, -0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 1.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{  0.0f,  1.0f,  0.0f }, glm::vec2{ 0.0f, 0.0f } },
            };

            geo.indices = {
                 2,  1,  0,
                 0,  3,  2,

                 4,  5,  6,
                 6,  7,  4,

                 8,  9, 10,
                10, 11,  8,

                14, 13, 12,
                12, 15, 14,

                16, 17, 18,
                18, 19, 16,

                22, 21, 20,
                20, 23, 22,
            };

            break;
        case Primitive::SQUARE:
            geometryName = "Square";

            geo.vertices = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            geo.indices = {
                2, 1, 0,
                3, 2, 0
            };

            break;
        }

        return std::make_pair(geo, geometryName);
    }
}