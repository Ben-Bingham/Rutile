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
        virtual void ProvideGeneralGUI() { }

        // Called after all default light settings, facilitates an ImGui gui
        virtual void ProvideDirectionalLightGUI() { }
        virtual void ProvidePointLightGUI(size_t i) { }

        // Replaces the entire old scene with a new one
        virtual void SetScene(Scene scene) { }

        // Optional callbacks inform the renderer of scene, camera, and window changes
        virtual void UpdateObjectTransform(size_t i, const glm::mat4& newTransform) { }
        virtual void UpdateObjectMaterial(size_t i, const Material newMaterial) { }

        virtual void UpdateDirectionalLight(const std::shared_ptr<DirectionalLight> newLight) { }
        virtual void UpdatePointLight(size_t i, const PointLight& newLight) { }

        virtual void UpdateViewportSize() { }
	};
}