#include "SceneGui.h"
#include "imgui.h"
#include <iostream>

#include <glm/gtc/type_ptr.inl>

#include "Settings/App.h"

namespace Rutile {
    void MainSceneGui() {
        const char* original = "Original";
        const char* triangle = "Triangle";

        const char* items[] = { original, triangle };
        static int currentIndex = 0;

        if (ImGui::BeginCombo("Select a Scene", items[currentIndex])) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool isSelected = (currentIndex == n);

                if (ImGui::Selectable(items[n], isSelected)) {
                    currentIndex = n;

                    if (std::string{ items[currentIndex] } == std::string{ original }) {
                        App::currentSceneType = SceneType::ORIGINAL_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ triangle }) {
                        App::currentSceneType = SceneType::TRIANGLE_SCENE;
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Scene Objects");
        ImGui::Separator();

        SceneObjects();

        ImGui::Separator();
    }

    void SceneObjects() {
        if (ImGui::TreeNode("Packets")) {
            for (Packet packet : App::scene.packets) {
                DisplayMaterial(packet.materialType, packet.material);

                DisplayTransform(packet.transform);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lights")) {
            for (size_t i = 0; i < App::scene.lights.size(); ++i) {
                DisplayLight(i, App::scene.lightTypes[i], App::scene.lights[i]);
            }

            ImGui::TreePop();
        }
    }

    void DisplayMaterial(MaterialType type, Material* material) {
        
    }

    void DisplayTransform(Transform* transform) {
        
    }

    void DisplayLight(size_t i, LightType type, Light* light) {
        std::string lightName{ };
        switch (type) {
            case LightType::POINT: {
                lightName = "Point Light";
                break;
            }
            case LightType::DIRECTION: {
                lightName = "Directional Light";
                break;
            }
            case LightType::SPOTLIGHT: {
                lightName = "Spot Light";
                break;
            }
        }

        if (ImGui::TreeNode(lightName.c_str())) {
            switch (type) {
                case LightType::POINT: {
                    PointLight* l = dynamic_cast<PointLight*>(light);

                    ImGui::DragFloat3(("Position##"                         + std::to_string(i)).c_str(), glm::value_ptr(l->position),  0.05f);

                    ImGui::DragFloat (("Constant Attenuation Component##"   + std::to_string(i)).c_str(), &l->constant,                 0.005f, 0.0f, 1.0f);
                    ImGui::DragFloat (("Linear Attenuation Component##"     + std::to_string(i)).c_str(), &l->linear,                   0.005f, 0.0f, 1.0f);
                    ImGui::DragFloat (("Quadratic Attenuation Component##"  + std::to_string(i)).c_str(), &l->quadratic,                0.005f, 1.0f);

                    ImGui::ColorEdit3(("Ambient Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->ambient));
                    ImGui::ColorEdit3(("Diffuse Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->diffuse));
                    ImGui::ColorEdit3(("Specular Color##"                   + std::to_string(i)).c_str(), glm::value_ptr(l->specular));
                    break;
                }
                case LightType::DIRECTION: {
                    DirectionalLight* l = dynamic_cast<DirectionalLight*>(light);

                    ImGui::DragFloat3(("Direction##"        + std::to_string(i)).c_str(), glm::value_ptr(l->direction), 0.05f);

                    ImGui::ColorEdit3(("Ambient Color##"    + std::to_string(i)).c_str(), glm::value_ptr(l->ambient));
                    ImGui::ColorEdit3(("Diffuse Color##"    + std::to_string(i)).c_str(), glm::value_ptr(l->diffuse));
                    ImGui::ColorEdit3(("Specular Color##"   + std::to_string(i)).c_str(), glm::value_ptr(l->specular));

                    break;
                }
                case LightType::SPOTLIGHT: {
                    SpotLight* l = dynamic_cast<SpotLight*>(light);

                    ImGui::DragFloat3(("Position##"                         + std::to_string(i)).c_str(), glm::value_ptr(l->position),  0.05f);
                    ImGui::DragFloat3(("Direction##"                        + std::to_string(i)).c_str(), glm::value_ptr(l->direction), 0.05f);

                    float cutOff = glm::degrees(glm::acos(l->cutOff));
                    float outerCutOff = glm::degrees(glm::acos(l->outerCutOff));

                    ImGui::DragFloat (("Inner Cut Off##"                    + std::to_string(i)).c_str(), &cutOff,                      0.5f, 0.0f, 180.0f);
                    ImGui::DragFloat (("Outer Cut Off##"                    + std::to_string(i)).c_str(), &outerCutOff,                 0.5f, 0.0f, 180.0f);

                    l->cutOff = glm::cos(glm::radians(cutOff));
                    l->outerCutOff = glm::cos(glm::radians(outerCutOff));

                    ImGui::DragFloat (("Constant Attenuation Component##"   + std::to_string(i)).c_str(), &l->constant,                 0.005f, 0.0f, 1.0f);
                    ImGui::DragFloat (("Linear Attenuation Component##"     + std::to_string(i)).c_str(), &l->linear,                   0.005f, 0.0f, 1.0f);
                    ImGui::DragFloat (("Quadratic Attenuation Component##"  + std::to_string(i)).c_str(), &l->quadratic,                0.005f, 1.0f);

                    ImGui::ColorEdit3(("Ambient Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->ambient));
                    ImGui::ColorEdit3(("Diffuse Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->diffuse));
                    ImGui::ColorEdit3(("Specular Color##"                   + std::to_string(i)).c_str(), glm::value_ptr(l->specular));

                    break;
                }
            }
            ImGui::TreePop();
        }
    }
}