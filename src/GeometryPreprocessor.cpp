#include "GeometryPreprocessor.h"

namespace Rutile {
    Bundle GeometryPreprocessor::GetBundle(GeometryMode mode) {
        Bundle bundle = m_CurrentBundle;

        Clear(m_CurrentBundle);

        return bundle;
    }

    void GeometryPreprocessor::Add(Primitive primitive) {
        switch (primitive) {
        case Primitive::TRIANGLE:
            Packet packet;

            packet.vertexData = {
                //      Position                         Normal                         Color                          Uv
                Vertex{ glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.3f, 0.1f, 0.7f }, glm::vec2{ 0.0f, 0.0f } },
                Vertex{ glm::vec3{  0.0f,  0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.6f, 0.2f, 0.8f }, glm::vec2{ 0.0f, 0.5f } },
                Vertex{ glm::vec3{  0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.5f, 0.4f, 0.1f }, glm::vec2{ 1.0f, 0.0f } },
            };

            packet.indexData = {
                0, 1, 2
            };

            packet.highestSupportedMaterialType = MaterialType::FLAT;

            m_CurrentBundle.packets.push_back(packet);
            m_CurrentBundle.packetQuantities.push_back(1);
            m_CurrentBundle.transforms.push_back(std::vector{ glm::mat4{ 1.0f } });

            break;
        }
    }
}