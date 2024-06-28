#include "GlobalRendererOptionsGui.h"
#include "imgui.h"

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

        ImGui::Text(("Current Renderer: " + rendererTypeName).c_str());

        ImGui::Separator();

        if (ImGui::Button("Restart Renderer")) {
            App::restartRenderer = true;
        }

        ImGui::Separator();

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
            if (ImGui::DragFloat("Bias", &App::settings.shadowMapBias, 0.0001f)) { App::renderer->UpdateShadowMapBias(); }
            ImGui::TreePop();
        }
    }
}