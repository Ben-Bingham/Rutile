#include "MainGuiWindow.h"
#include "imgui.h"
#include <iostream>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "Scenes/MainSceneGui.h"

#include "Settings/App.h"

namespace Rutile {
    void MainGuiWindow() {
        ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        ImGui::Begin("Rutile");
        {
            if (ImGui::CollapsingHeader("Global Renderer Options")) {
                if (ImGui::Button("Restart Renderer")) {
                    App::restartRenderer = true;
                }
            }

            if (ImGui::CollapsingHeader("Local Renderer Options")) {}

            if (ImGui::CollapsingHeader("Scene Options")) {
                MainSceneGui();
            }

            ImGui::Text("Renderer");

            if (ImGui::CollapsingHeader("Lights")) {
                int pointLightCount = 1;
                int directionalLightCount = 1;
                int spotLightCount = 1;
                int i = 0;
                for (auto lightType : App::scene.lightTypes) {
                    switch (lightType) {
                    case LightType::POINT: {
                        PointLight* light = dynamic_cast<PointLight*>(App::scene.lights[i]);
                        if (ImGui::TreeNode(("Point light #" + std::to_string(pointLightCount)).c_str())) {
                            ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(light->position), 0.05f);

                            if (ImGui::DragFloat(("Constant Attenuation Component##" + std::to_string(i)).c_str(), &light->constant, 0.005f, 0.0f, 1.0f)) {
                                std::cout << "DRAGGING " << std::endl;
                            }
                            ImGui::DragFloat(("Linear Attenuation Component##" + std::to_string(i)).c_str(), &light->linear, 0.005f, 0.0f, 1.0f);
                            ImGui::DragFloat(("Quadratic Attenuation Component##" + std::to_string(i)).c_str(), &light->quadratic, 0.005f, 1.0f);

                            ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                            ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                            ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                            ImGui::TreePop();
                        }

                        ++pointLightCount;
                        break;
                    }
                    case LightType::DIRECTION: {
                        DirectionalLight* light = dynamic_cast<DirectionalLight*>(App::scene.lights[i]);
                        if (ImGui::TreeNode(("Directional light #" + std::to_string(directionalLightCount)).c_str())) {
                            ImGui::DragFloat3(("Direction##" + std::to_string(i)).c_str(), glm::value_ptr(light->direction), 0.05f);

                            ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                            ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                            ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                            ImGui::TreePop();
                        }

                        ++directionalLightCount;
                        break;
                    }
                    case LightType::SPOTLIGHT: {
                        SpotLight* light = dynamic_cast<SpotLight*>(App::scene.lights[i]);
                        if (ImGui::TreeNode(("Spotlight #" + std::to_string(spotLightCount)).c_str())) {
                            ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(light->position), 0.05f);
                            ImGui::DragFloat3(("Direction##" + std::to_string(i)).c_str(), glm::value_ptr(light->direction), 0.05f);

                            float cutOff = glm::degrees(glm::acos(light->cutOff));
                            float outerCutOff = glm::degrees(glm::acos(light->outerCutOff));

                            ImGui::DragFloat(("Inner Cut Off##" + std::to_string(i)).c_str(), &cutOff, 0.5f, 0.0f, 180.0f);
                            ImGui::DragFloat(("Outer Cut Off##" + std::to_string(i)).c_str(), &outerCutOff, 0.5f, 0.0f, 180.0f);

                            light->cutOff = glm::cos(glm::radians(cutOff));
                            light->outerCutOff = glm::cos(glm::radians(outerCutOff));

                            ImGui::DragFloat(("Constant Attenuation Component##" + std::to_string(i)).c_str(), &light->constant, 0.005f, 0.0f, 1.0f);
                            ImGui::DragFloat(("Linear Attenuation Component##" + std::to_string(i)).c_str(), &light->linear, 0.005f, 0.0f, 1.0f);
                            ImGui::DragFloat(("Quadratic Attenuation Component##" + std::to_string(i)).c_str(), &light->quadratic, 0.005f, 1.0f);

                            ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                            ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                            ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                            ImGui::TreePop();
                        }
                        ++spotLightCount;
                        break;
                    }
                    }
                    ++i;
                }
            }

            if (ImGui::CollapsingHeader("Materials")) {
                std::vector<std::pair<MaterialType, Material*>> materials;
                for (auto packet : App::scene.packets) {
                    MaterialType type = packet.materialType;
                    Material* material = packet.material;

                    bool alreadyHave = false;
                    for (auto mat : materials) {
                        if (mat.first == type && mat.second == material) {
                            alreadyHave = true;
                            break;
                        }
                    }

                    if (!alreadyHave) {
                        materials.push_back(std::make_pair(type, material));
                    }
                }

                int solidMaterialCount = 1;
                int phongMaterialCount = 1;
                int i = 0;
                for (auto mat : materials) {
                    switch (mat.first) {
                    case MaterialType::SOLID: {
                        Solid* material = dynamic_cast<Solid*>(mat.second);
                        if (ImGui::TreeNode(("Solid Material #" + std::to_string(solidMaterialCount)).c_str())) {
                            ImGui::ColorEdit3(("Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->color));

                            ImGui::TreePop();
                        }
                        ++solidMaterialCount;
                        break;
                    }
                    case MaterialType::PHONG: {
                        Phong* material = dynamic_cast<Phong*>(mat.second);
                        if (ImGui::TreeNode(("Phong Material #" + std::to_string(phongMaterialCount)).c_str())) {
                            ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->ambient));
                            ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->diffuse));
                            ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->specular));

                            ImGui::DragFloat(("Shininess##" + std::to_string(i)).c_str(), &material->shininess, 0.5f, 0.0f, 10000.0f);

                            ImGui::TreePop();
                        }
                        ++phongMaterialCount;
                        break;
                    }
                    }
                    ++i;
                }
            }

            /*
            if (ImGui::CollapsingHeader("Transforms")) {
                int transformCount = 1;
                int i = 0;
                for (auto t : App::scene.transforms) {
                    glm::mat4* transform = t[0];

                    if (ImGui::TreeNode(("Transform #" + std::to_string(transformCount)).c_str())) {
                        glm::vec3 translation = { (*transform)[3][0], (*transform)[3][1], (*transform)[3][2] };
                        glm::vec3 translationBackup = translation;
                        ImGui::DragFloat3(("Translation##" + std::to_string(i)).c_str(), glm::value_ptr(translation), 0.01f);
                        *transform = glm::translate(*transform, translation - translationBackup);

                        glm::vec3 scaling = { (*transform)[0][0], (*transform)[1][1], (*transform)[2][2] };
                        glm::vec3 scalingBackup = scaling - 1.0f;
                        ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), glm::value_ptr(scaling), 0.01f, 0.1f, 10000.0f);
                        *transform = glm::scale(*transform, scaling - scalingBackup);

                        ImGui::TreePop();
                    }
                    ++transformCount;
                    ++i;
                }
            }
            */
        }
        ImGui::End();
    }
}