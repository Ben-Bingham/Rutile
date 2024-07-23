#include "ShadowSettings.h"
#include "imgui.h"
#include "ImGuiUtil.h"

#include "Settings/App.h"

#include "Utility/events/Events.h"

namespace Rutile {
    void ShadowSettings() {
        // Directional
        ImGui::Text("Directional Shadow maps");

        ImGui::Separator();

        if (ImGui::Checkbox("Enable##dir", &App::settings.directionalShadows)) { App::renderer->SignalDirectionalShadowMapUpdate(); }

        if (App::settings.directionalShadows) {
            ImGui::Separator();

            RadioButtons("Bias mode", { "None##biasMode", "Static##biasMode", "Dynamic##biasMode" }, (int*)&App::settings.shadowMapBiasMode, [] { App::eventManager.Notify(new DirectionalShadowMapUpdate{ }); });

            if (App::settings.shadowMapBiasMode == ShadowMapBiasMode::STATIC) {
                if (ImGui::DragFloat("Bias##dir", &App::settings.directionalShadowMapBias, 0.0001f)) { App::renderer->SignalDirectionalShadowMapUpdate(); }
            }
            else if (App::settings.shadowMapBiasMode == ShadowMapBiasMode::DYNAMIC) {
                if (ImGui::DragFloat("Bias Minimum", &App::settings.directionalDynamicShadowMapBiasMin, 0.0001f)) { App::renderer->SignalDirectionalShadowMapUpdate(); }
                if (ImGui::DragFloat("Bias Maximum", &App::settings.directionalDynamicShadowMapBiasMax, 0.0001f)) { App::renderer->SignalDirectionalShadowMapUpdate(); }
            }

            ImGui::Separator();

            RadioButtons("Culled Face During Directional Shadow map Rendering", { "Front##dir", "Back##dir" }, (int*)&App::settings.culledFaceDuringDirectionalShadowMapping, [] { App::eventManager.Notify(new DirectionalShadowMapUpdate{ }); });

            ImGui::Separator();

            ImGui::Checkbox("Enable PCF##dir", &App::settings.directionalShadowMapPCF);
        }

        // Cascading Shadow maps
        ImGui::Text("Cascading Shadow maps");

        ImGui::Separator();

        ImGui::Checkbox("Cascade Visualization", &App::settings.visualizeCascades);
        ImGui::Checkbox("Cascade Light Visualization", &App::settings.visualizeCascadeLights);

        ImGui::Checkbox("Lock Cascade Camera", &App::settings.lockCascadeCamera);

        App::renderer->ProvideCSMVisualization();

        // Omnidirectional
        ImGui::Separator();

        ImGui::Text("Omnidirectional Shadow maps");

        ImGui::Separator();

        if (ImGui::Checkbox("Enable##omni", &App::settings.omnidirectionalShadowMaps)) { App::renderer->SignalOmnidirectionalShadowMapUpdate(); }

        if (App::settings.omnidirectionalShadowMaps) {
            ImGui::Separator();

            if (ImGui::DragFloat("Bias##omni", &App::settings.omnidirectionalShadowMapBias, 0.0001f)) { App::renderer->SignalOmnidirectionalShadowMapUpdate(); }

            ImGui::Separator();

            RadioButtons("Culled Face During Omnidirectional Shadow map Rendering", { "Front##omni", "Back##omni" }, (int*)&App::settings.culledFaceDuringOmnidirectionalShadowMapping, [] { App::eventManager.Notify(new OmnidirectionalShadowMapUpdate{ }); });

            ImGui::Separator();

            RadioButtons("Omnidirectional Shadow maps PCF mode", { "No PCF##omni", "Fixed Sample Count", "Fixed Sample Directions" }, (int*)&App::settings.omnidirectionalShadowMapPCFMode, [] { App::eventManager.Notify(new OmnidirectionalShadowMapUpdate{ }); });

            if (App::settings.omnidirectionalShadowMapPCFMode == OmnidirectionalShadowMapPCFMode::FIXED_SAMPLE_COUNT) {
                if (ImGui::DragInt("Sample Count", &App::settings.omnidirectionalShadowMapSampleCount, 0.01f)) { App::renderer->SignalOmnidirectionalShadowMapUpdate(); }
            } else if (App::settings.omnidirectionalShadowMapPCFMode == OmnidirectionalShadowMapPCFMode::FIXED_SAMPLE_DIRECTIONS) {
                RadioButtons("Disk Radius mode", { "Static##omni", "Vary with Distance" }, (int*)&App::settings.omnidirectionalShadowMapDiskRadiusMode, [] { App::eventManager.Notify(new OmnidirectionalShadowMapUpdate{ }); });

                if (App::settings.omnidirectionalShadowMapDiskRadiusMode == OmnidirectionalShadowMapDiskRadiusMode::STATIC) {
                    if (ImGui::DragFloat("Disk Radius", &App::settings.omnidirectionalShadowMapDiskRadius, 0.001f)) { App::renderer->SignalOmnidirectionalShadowMapUpdate(); }
                }
            }
        }
    }
}