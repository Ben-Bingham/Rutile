#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "rendering/Scene.h"

namespace Rutile {
	class Renderer {
	public:
		Renderer() = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() = default;

        virtual GLFWwindow* Init() = 0;
        virtual void Cleanup(GLFWwindow* window) = 0;

        virtual void Render() = 0;

        virtual void SetScene(const Scene& scene) = 0;

        virtual void WindowResize();

        // These functions replace either a light or packet at a specified index within the current scene
            // A packet is considered invalid if it has no vertices
            // A light is considered invalid if it is a nullptr
        virtual void SetPacket(size_t index, Packet packet);
        virtual void SetLight(size_t index, LightType type, Light* light);

        // These functions add on a NEW packet/light onto the current scene
        virtual void AddPacket(Packet packet);
        virtual void AddLight(LightType type, Light* light);

        // These functions are called when a value within either the material or transform of a packet are modified
        virtual void UpdatePacketMaterial(size_t index);
        virtual void UpdatePacketTransform(size_t index);
    };
}