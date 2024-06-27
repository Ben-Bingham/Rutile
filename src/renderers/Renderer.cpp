#include "Renderer.h"

namespace Rutile {
    void Renderer::WindowResize() { }

    void Renderer::SetPacket(size_t index, Packet packet) { }
    void Renderer::SetLight(size_t index, LightType type, Light* light) { }

    void Renderer::AddPacket(Packet packet) { }
    void Renderer::AddLight(LightType type, Light* light) { }

    void Renderer::UpdatePacketMaterial(size_t index) { }
    void Renderer::UpdatePacketTransform(size_t index) { }
    void Renderer::UpdateSceneLight(size_t index) { }
}