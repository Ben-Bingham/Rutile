#include "SceneFactory.h"

namespace Rutile {
    Scene SceneFactory::GetScene() {
        return m_CurrentScene;
    }

    void SceneFactory::Add(Primitive primitive, Transform* transform, MaterialType materialType, Material* material) {
        Packet packet;
        switch (primitive) {
        case Primitive::TRIANGLE:

            packet.vertexData = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.0f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.5f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            packet.indexData = {
                0, 1, 2
            };

            packet.materialType = materialType;
            packet.material = material;

            packet.transform = transform;

            m_CurrentScene.packets.push_back(packet);

            break;

        case Primitive::CUBE:
            packet.vertexData = {
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

            packet.indexData = {
                 0,  1,  2,
                 2,  3,  0,

                 4,  5,  6,
                 6,  7,  4,

                 8,  9, 10,
                10, 11,  8,

                12, 13, 14,
                14, 15, 12,

                16, 17, 18,
                18, 19, 16,

                20, 21, 22,
                22, 23, 20,
            };

            packet.materialType = materialType;
            packet.material = material;

            packet.transform = transform;

            m_CurrentScene.packets.push_back(packet);

            break;
        case Primitive::SQUARE:
            packet.vertexData = {
                //      Position                         Normal                         Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{ -0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
            };

            packet.indexData = {
                0, 1, 2,
                0, 2, 3
            };

            packet.materialType = materialType;
            packet.material = material;

            packet.transform = transform;

            m_CurrentScene.packets.push_back(packet);

            break;
        }
    }

    void SceneFactory::Add(LightType type, Light* light) {
        m_CurrentScene.lightTypes.push_back(type);
        m_CurrentScene.lights.push_back(light);
    }
}