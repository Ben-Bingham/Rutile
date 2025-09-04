#pragma once

#include <memory>

#include "Scene/Scene.h"
#include "Utility/OpenGl/Texture.h"
#include "Utility/OpenGl/Framebuffer.h"

namespace Rutile {
	class Renderer {
	public:
        Renderer() = default;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = delete;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() = default;

        // Called every frame, the renderer should render to the given framebuffer
        virtual void Render(Framebuffer& framebuffer) = 0; // TODO maybe the frambebuffer is just bound and unbound before and after this call?

        // Called every frame, facilitates an ImGui gui
        virtual void ProvideGUI() { }

        // Replaces the entire old scene with a new one
        virtual void SetScene(Scene& scene) { }

        // Optional callbacks inform the renderer of scene, camera, and window changes
        virtual void UpdateObjectTransform() { }
        virtual void UpdateObjectMaterial() { }

        virtual void UpdateSceneLight() { }

        virtual void UpdateCameraPosition() { }
        virtual void UpdateCameraFOV() { }

        virtual void UpdateWindowSize() { };
	};
}