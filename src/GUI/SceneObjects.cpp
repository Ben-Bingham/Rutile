#include "SceneObjects.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Settings/App.h"

namespace Rutile {
    void SceneObjects() {
        if (ImGui::TreeNode("Objects")) {
            int i = 0;
            for (auto object : App::scene.objects) {
                if (ImGui::TreeNode((object.name + "##SceneObjects" + std::to_string(i)).c_str())) {
                    ImGui::Text(("Geometry: " + App::geometryBank.GetName(i)).c_str());

                    ImGui::Text("Transform");

                    Transform& transform = App::transformBank[object.transform];

                    if (ImGui::DragFloat3(("Translation##" + std::to_string(i)).c_str(), glm::value_ptr(transform.position), 0.01f)) { App::renderer->SignalObjectTransformUpdate(i); App::transformBank[object.transform].CalculateMatrix(); }
                    if (ImGui::DragFloat3(("Scale##"       + std::to_string(i)).c_str(), glm::value_ptr(transform.scale),    0.01f)) { App::renderer->SignalObjectTransformUpdate(i); App::transformBank[object.transform].CalculateMatrix(); }
                    if (ImGui::DragFloat4(("Rotation##"    + std::to_string(i)).c_str(), glm::value_ptr(transform.rotation), 0.01f)) { App::renderer->SignalObjectTransformUpdate(i); App::transformBank[object.transform].CalculateMatrix(); }

                    ImGui::Text("Material");
                    switch (App::settings.materialType) {
                        case MaterialType::SOLID: {
                            if (ImGui::ColorEdit3(("Color##material" + std::to_string(object.material)).c_str(), glm::value_ptr(dynamic_cast<Solid*>(App::materialBank[object.material])->color))) { App::renderer->SignalObjectMaterialUpdate(i); }

                            break;
                        } 
                        case MaterialType::PHONG: {
                            if (ImGui::ColorEdit3(("Ambient##material"  + std::to_string(object.material)).c_str(), glm::value_ptr(dynamic_cast<Phong*>(App::materialBank[object.material])->ambient)))  { App::renderer->SignalObjectMaterialUpdate(i); }
                            if (ImGui::ColorEdit3(("Diffuse##material"  + std::to_string(object.material)).c_str(), glm::value_ptr(dynamic_cast<Phong*>(App::materialBank[object.material])->diffuse)))  { App::renderer->SignalObjectMaterialUpdate(i); }
                            if (ImGui::ColorEdit3(("Specular##material" + std::to_string(object.material)).c_str(), glm::value_ptr(dynamic_cast<Phong*>(App::materialBank[object.material])->specular))) { App::renderer->SignalObjectMaterialUpdate(i); }

                            if (ImGui::DragFloat(("Shininess##material" + std::to_string(object.material)).c_str(), &dynamic_cast<Phong*>(App::materialBank[object.material])->shininess))               { App::renderer->SignalObjectMaterialUpdate(i); }

                            break;
                        }
                    }

                    ImGui::TreePop();
                }
                ++i;
            }
            ImGui::TreePop();
        }

        if (App::settings.materialType != MaterialType::SOLID) {
            if (ImGui::TreeNode("Lights")) {
                if (App::scene.HasDirectionalLight()) {
                    if (ImGui::TreeNode("Directional Light")) {
                        if (ImGui::DragFloat3("Direction##Dir", glm::value_ptr(App::scene.directionalLight.direction), 0.05f)) { App::renderer->SignalDirectionalLightUpdate(); }

                        if (ImGui::ColorEdit3("Ambient Color##Dir", glm::value_ptr(App::scene.directionalLight.ambient))) { App::renderer->SignalDirectionalLightUpdate(); }
                        if (ImGui::ColorEdit3("Diffuse Color##Dir", glm::value_ptr(App::scene.directionalLight.diffuse))) { App::renderer->SignalDirectionalLightUpdate(); }
                        if (ImGui::ColorEdit3("Specular Color##Dir", glm::value_ptr(App::scene.directionalLight.specular))) { App::renderer->SignalDirectionalLightUpdate(); }

                        App::renderer->ProvideDirectionalLightVisualization();

                        ImGui::TreePop();
                    }
                }

                if (!App::scene.pointLights.empty()) {
                    LightIndex lightIndex = 0;
                    for (auto& pointLight : App::scene.pointLights) {
                        if (ImGui::TreeNode(("Point light #" + std::to_string(lightIndex + 1)).c_str())) {
                            if (ImGui::DragFloat3(("Position##Point" + std::to_string(lightIndex)).c_str(), glm::value_ptr(pointLight.position), 0.05f)) { App::renderer->SignalPointLightUpdate(lightIndex); }

                            if (ImGui::DragFloat(("Constant Attenuation Component##"  + std::to_string(lightIndex)).c_str(), &pointLight.constant,  0.005f, 0.0f, 1.0f)) { App::renderer->SignalPointLightUpdate(lightIndex); }
                            if (ImGui::DragFloat(("Linear Attenuation Component##"    + std::to_string(lightIndex)).c_str(), &pointLight.linear,    0.005f, 0.0f, 1.0f)) { App::renderer->SignalPointLightUpdate(lightIndex); }
                            if (ImGui::DragFloat(("Quadratic Attenuation Component##" + std::to_string(lightIndex)).c_str(), &pointLight.quadratic, 0.005f, 0.0f, 1.0f)) { App::renderer->SignalPointLightUpdate(lightIndex); }

                            if (ImGui::ColorEdit3(("Ambient Color##"  + std::to_string(lightIndex)).c_str(), glm::value_ptr(pointLight.ambient)))  { App::renderer->SignalPointLightUpdate(lightIndex); }
                            if (ImGui::ColorEdit3(("Diffuse Color##"  + std::to_string(lightIndex)).c_str(), glm::value_ptr(pointLight.diffuse)))  { App::renderer->SignalPointLightUpdate(lightIndex); }
                            if (ImGui::ColorEdit3(("Specular Color##" + std::to_string(lightIndex)).c_str(), glm::value_ptr(pointLight.specular))) { App::renderer->SignalPointLightUpdate(lightIndex); }

                            ImGui::Text("Frustum");
                            ImGui::DragFloat("Near Plane Distance##pointLight", &pointLight.shadowMapNearPlane, 0.1f);
                            ImGui::DragFloat("Far Plane Distance##pointLight", &pointLight.shadowMapFarPlane, 0.1f);

                            if (App::settings.omnidirectionalShadowMaps) {
                                App::renderer->ProvideLightVisualization(lightIndex);
                            }

                            ImGui::TreePop();
                        }
                        ++lightIndex;
                    }
                }

                ImGui::TreePop();
            }
        }
    }
}