#pragma once

#include <memory>

#include "Scene/Scene.h"
#include "Utility/OpenGl/Texture.h"

namespace Rutile {
	class Renderer {
	public:
        Renderer() = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() = default;

        // Called when renderer is first needed
        virtual void Initialize(Scene& scene) { };

        // Called every frame, the returned texture should contain the output image
        virtual std::shared_ptr<Texture2D> Render() = 0;

        // Called when renderer is no longer needed
        virtual void Destroy() { };

        // Called every frame, facilitates an ImGui gui
        virtual void ProvideGUI() { };

        // Optional callbacks inform the renderer of scene, camera, and windows changes
        virtual void UpdateObjectTransform() { };
        virtual void UpdateObjectMaterial() { };

        virtual void UpdateCameraPosition() { };
        virtual void UpdateCameraFOV() { };

        virtual void UpdateWindowSize() { };
	};
}