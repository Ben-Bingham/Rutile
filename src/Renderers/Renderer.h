#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "RenderingAPI/Scene.h"

#include "Utility/events/EventListener.h"

namespace Rutile {
	class Renderer : public EventListener {
	public:
		Renderer() = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
        virtual ~Renderer() override = default;

        // Necessities
        virtual GLFWwindow* Init() = 0;
        virtual void Cleanup(GLFWwindow* window) = 0;

        virtual void Render() = 0;

        virtual void LoadScene() = 0;

        virtual void ProjectionMatrixUpdate() { }

        virtual void SignalObjectGeometryUpdate(ObjectIndex i)  { }

        virtual void SignalDirectionalLightUpdate() { }
        virtual void SignalPointLightUpdate(LightIndex i) { }

        // Global settings updates
        virtual void SignalSettingsUpdate() { }

        virtual void SignalGeneralShadowMapUpdate()         { }
        virtual void SignalDirectionalShadowMapUpdate()     { }
        virtual void SignalOmnidirectionalShadowMapUpdate() { }
        virtual void SignalRayTracingSettingsChange()       { }

        // Material settings updates
        virtual void SignalSolidMaterialUpdate() { }
        virtual void SignalPhongMaterialUpdate() { }

        // ImGui visualizations
        virtual void ProvideLightVisualization(LightIndex lightIndex) { }
        virtual void ProvideDirectionalLightVisualization() { }

        virtual void ProvideCSMVisualization() { }

        virtual void ProvideTimingStatistics() { }
        virtual void ProvideLocalRendererSettings() { }
	};
}