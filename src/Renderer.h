#pragma once

#include <memory>

#include "Scene/Scene.h"
#include "Scene/Camera.h"

#include "Utility/OpenGl/RenderTarget.h"

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
        virtual void Render(RenderTarget& target, const Camera& camera) = 0;

        // Called every frame, facilitates an ImGui gui
        virtual void ProvideGUI() { }

        // Replaces the entire old scene with a new one
        virtual void SetScene(Scene& scene) { }

        // Optional callbacks inform the renderer of scene, camera, and window changes
        virtual void UpdateObjectTransform() { }
        virtual void UpdateObjectMaterial() { }

        virtual void UpdateSceneLight() { }

        virtual void UpdateCameraPosition() { }
        virtual void UpdateCameraDirection() { }
        virtual void UpdateCameraFOV() { }

        virtual void UpdateWindowSize() { };
	};
}