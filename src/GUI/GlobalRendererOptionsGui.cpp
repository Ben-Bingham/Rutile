#include "GlobalRendererOptionsGui.h"
#include "imgui.h"
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Settings/App.h"

namespace Rutile {
    void GlobalRendererOptionsGui() {
        std::string rendererTypeName{ };
        switch (App::currentRendererType) {
            case RendererType::OPENGL: {
                rendererTypeName = "OpenGl Renderer";
                break;
            }
        }

        const char* defaultPreset = "Default";
        const char* shadowMapTesting = "Shadow Map Testing Scene";

        const char* items[] = { defaultPreset, shadowMapTesting };
        static int currentIndex = 0;

        if (ImGui::BeginCombo("Setting presets", items[currentIndex])) {
            bool optionChanged = false;
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool isSelected = (currentIndex == n);

                if (ImGui::Selectable(items[n], isSelected)) {
                    currentIndex = n;

                    if (std::string{ items[currentIndex] } == std::string{ defaultPreset }) {
                        App::settings = DefaultSettings();
                        optionChanged = true;
                    }
                    else if (std::string{ items[currentIndex] } == std::string{ shadowMapTesting }) {
                        App::settings = ShadowMapTestingSceneSettings();
                        optionChanged = true;
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            if (optionChanged) {
                App::renderer->UpdateFieldOfView();
                App::renderer->UpdateNearPlane();
                App::renderer->UpdateFarPlane();

                App::renderer->UpdateShadowMap();
            }

            ImGui::EndCombo();
        }


        ImGui::Text(("Current Renderer: " + rendererTypeName).c_str());

        ImGui::Separator();

        if (ImGui::Button("Restart Renderer")) {
            App::restartRenderer = true;
        }

        ImGui::Separator();

        if (ImGui::TreeNode("Generic Settings")) {
            ImGui::Text("Culled face during rendering");
            ImGui::RadioButton("Front##renderMode", (int*)&App::settings.culledFaceDuringRendering, 0); ImGui::SameLine();
            ImGui::RadioButton("Back##renderMode",  (int*)&App::settings.culledFaceDuringRendering, 1);

            ImGui::Text("Front face winding order");
            ImGui::RadioButton("Clock-wise",         (int*)&App::settings.frontFace, 0); ImGui::SameLine();
            ImGui::RadioButton("Counter-clock-wise", (int*)&App::settings.frontFace, 1);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Frustum Settings")) {
            if (ImGui::DragFloat("Field of View",       &App::settings.fieldOfView, 0.1f, 0.0f, 180.0f))      { App::renderer->UpdateFieldOfView(); }
            if (ImGui::DragFloat("Near Clipping Plane", &App::settings.nearPlane,   0.1f, 0.0f, 10000000.0f)) { App::renderer->UpdateNearPlane();   }
            if (ImGui::DragFloat("Far Clipping Plane",  &App::settings.farPlane,    0.1f, 0.0f, 10000000.0f)) { App::renderer->UpdateFarPlane();    }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera Settings")) {
            if (ImGui::DragFloat3("Position##camera",           glm::value_ptr(App::camera.position),    0.1f                  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }
            if (ImGui::DragFloat3("Front Vector##camera",       glm::value_ptr(App::camera.frontVector), 0.1f                  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }
            if (ImGui::DragFloat3("Up Vector##camera",          glm::value_ptr(App::camera.upVector),    0.1f                  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }
            if (ImGui::DragFloat3("Right Vector##camera",       glm::value_ptr(App::camera.rightVector), 0.1f                  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }

            if (ImGui::DragFloat ("Yaw##camera",                &App::camera.yaw,                        0.1f                  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }
            if (ImGui::DragFloat ("Pitch##camera",              &App::camera.pitch,                      0.1f,  -89.9f, 89.9f  )) { App::renderer->UpdateCamera(); App::updateCameraVectors = true; }

            if (ImGui::DragFloat ("Mouse Sensitivity##camera",  &App::camera.lookSensitivity,            0.01f, 0.0f,   100.0f )) { App::renderer->UpdateCamera(); }
            if (ImGui::DragFloat ("Movement Speed##camera",     &App::camera.speed,                      0.1f,  0.0f,   1000.0f)) { App::renderer->UpdateCamera(); }


            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Shadow Map Settings")) {
            ImGui::Separator();
            ImGui::Text("Bias mode");
            if (ImGui::RadioButton("None##biasMode", (int*)&App::settings.shadowMapBiasMode, 0)) { App::renderer->UpdateShadowMap(); } ImGui::SameLine();
            if (ImGui::RadioButton("Static",         (int*)&App::settings.shadowMapBiasMode, 1)) { App::renderer->UpdateShadowMap(); } ImGui::SameLine();
            if (ImGui::RadioButton("Dynamic",        (int*)&App::settings.shadowMapBiasMode, 2)) { App::renderer->UpdateShadowMap(); }

            if (App::settings.shadowMapBiasMode == ShadowMapBiasMode::STATIC) {
                if (ImGui::DragFloat("Bias", &App::settings.shadowMapBias, 0.0001f)) { App::renderer->UpdateShadowMap(); }
            } else if (App::settings.shadowMapBiasMode == ShadowMapBiasMode::DYNAMIC) {
                if (ImGui::DragFloat("Dynamic Bias Minimum", &App::settings.dynamicShadowMapBiasMin, 0.0001f)) { App::renderer->UpdateShadowMap(); }
                if (ImGui::DragFloat("Dynamic Bias Maximum", &App::settings.dynamicShadowMapBiasMax, 0.0001f)) { App::renderer->UpdateShadowMap(); }
            }
            ImGui::Separator();

            ImGui::Text("Culled face during shadow map rendering");
            ImGui::RadioButton("Front##shadowMapMode", (int*)&App::settings.culledFaceDuringShadowMapping, 0); ImGui::SameLine();
            ImGui::RadioButton("Back##shadowMapMode",  (int*)&App::settings.culledFaceDuringShadowMapping, 1);

            ImGui::Separator();

            ImGui::Text("PCF mode");
            if (ImGui::RadioButton("None##pcfMode",                          (int*)&App::settings.shadowMapPcfMode, 0)) { App::renderer->UpdateShadowMap(); } ImGui::SameLine();
            if (ImGui::RadioButton("Sampling from adjacent texels", (int*)&App::settings.shadowMapPcfMode, 1)) { App::renderer->UpdateShadowMap(); }

            ImGui::TreePop();
        }
    }
}