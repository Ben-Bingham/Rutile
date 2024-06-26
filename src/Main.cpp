#include <iostream>

#include "GeometryPreprocessor.h"
#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "App.h"
#include "imgui.h"

#include <string>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "rendering/Camera.h"
#include "rendering/Material.h"

using namespace Rutile;

void CreateCurrentRenderer(App::RendererType type) {
    switch (App::currentRendererType) {
    case App::RendererType::OPENGL:
        App::renderer = std::make_unique<OpenGlRenderer>();
        break;
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    App::window = App::renderer->Init();
    App::renderer->SetBundle(App::bundle);

    App::glfw.AttachOntoWindow(App::window);

    App::imGui.Init(App::window);
}

void ShutDownCurrentRenderer() {
    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);
    App::renderer.reset();
}

int main() {
    App::glfw.Init();

    GeometryPreprocessor geometryPreprocessor{ };
    // Setting up bundle
    {
        Solid solid;
        solid.color = { 1.0f, 0.0f, 1.0f };

        Solid solid2;
        solid2.color = { 0.2f, 0.5f, 0.7f };

        Phong phong;
        phong.ambient = { 1.0f, 0.5f, 0.31f };
        phong.diffuse = { 1.0f, 0.5f, 0.31f };
        phong.specular = { 0.5f, 0.5f, 0.5f };
        phong.shininess = 32.0f;

        Phong phong2;
        //phong2.ambient

        glm::mat4 transform1 = glm::mat4{ 1.0f };
        transform1 = glm::translate(transform1, glm::vec3{ 1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, &transform1, MaterialType::SOLID, &solid);

        glm::mat4 transform2 = glm::mat4{ 1.0f };
        transform2 = glm::translate(transform2, glm::vec3{ -1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, &transform2, MaterialType::SOLID, &solid2);

        glm::mat4 transform3 = glm::mat4{ 1.0f };
        transform3 = glm::translate(transform3, glm::vec3{ 0.0f, 0.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::SQUARE, &transform3, MaterialType::SOLID, &solid);

        glm::mat4 transform4 = glm::mat4{ 1.0f };
        transform4 = glm::translate(transform4, glm::vec3{ 1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, &transform4, MaterialType::SOLID, &solid2);

        glm::mat4 transform5 = glm::mat4{ 1.0f };
        transform5 = glm::translate(transform5, glm::vec3{ -1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, &transform5, MaterialType::PHONG, &phong);

        PointLight pointLight;
        pointLight.position = { -2.0f, 2.0f, 2.0f };

        pointLight.ambient = { 0.05f, 0.05f, 0.05f };
        pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight.specular = { 1.0f, 1.0f, 1.0f };

        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        geometryPreprocessor.Add(LightType::POINT, &pointLight);

        DirectionalLight directionalLight;
        directionalLight.direction = { 0.0f, -1.0f, 0.0f };

        directionalLight.ambient = { 0.05f, 0.05f, 0.05f };
        directionalLight.diffuse = { 0.4f, 0.4f, 0.4f };
        directionalLight.specular = { 0.5f, 0.5f, 0.5f };

        geometryPreprocessor.Add(LightType::DIRECTION, &directionalLight);

        SpotLight spotLight;
        spotLight.position = { 0.0f, 0.0f, 0.0f };
        spotLight.direction = { 0.0f, 0.0f, -1.0f };

        spotLight.ambient = { 0.0f, 0.0f, 0.0f };
        spotLight.diffuse = { 1.0f, 1.0f, 1.0f };
        spotLight.specular = { 1.0f, 1.0f, 1.0f };

        spotLight.constant = 1.0f;
        spotLight.linear = 0.09f;
        spotLight.quadratic = 0.032f;

        spotLight.cutOff = glm::cos(glm::radians(12.5f));
        spotLight.outerCutOff = glm::cos(glm::radians(15.0f));

        geometryPreprocessor.Add(LightType::SPOTLIGHT, &spotLight);
    }
    App::bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

    CreateCurrentRenderer(App::currentRendererType);

    Camera camera;

    int lastMouseX = 0;
    int lastMouseY = 0;

    // Main loop
    while (!glfwWindowShouldClose(App::window)) {
        auto frameStart = std::chrono::system_clock::now();

        glfwPollEvents();
        // Movement
        {
            float dt = static_cast<float>(App::frameTime.count());
            float velocity = camera.speed * dt;
            if (glfwGetKey(App::window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.position += camera.frontVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.position -= camera.frontVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.position += camera.rightVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.position -= camera.rightVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.position += camera.upVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.position -= camera.upVector * velocity;
            }

            if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                if (App::mouseDown == false) {
                    lastMouseX = App::mousePosition.x;
                    lastMouseY = App::mousePosition.y;
                }

                App::mouseDown = true;
            }
            if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
                App::mouseDown = false;
            }

            if (App::mouseDown) {
                float xDelta = (float)App::mousePosition.x - (float)lastMouseX;
                float yDelta = (float)lastMouseY - (float)App::mousePosition.y; // reversed since y-coordinates go from bottom to top

                camera.yaw += xDelta * camera.lookSensitivity;
                camera.pitch += yDelta * camera.lookSensitivity;

                if (camera.pitch > 89.9f) {
                    camera.pitch = 89.9f;
                }
                else if (camera.pitch < -89.9f) {
                    camera.pitch = -89.9f;
                }

                camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
                camera.frontVector.y = sin(glm::radians(camera.pitch));
                camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
                camera.frontVector = glm::normalize(camera.frontVector);

                camera.rightVector = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

                lastMouseX = App::mousePosition.x;
                lastMouseY = App::mousePosition.y;
            }
        }

        if (App::restartRenderer) {
            ShutDownCurrentRenderer();

            CreateCurrentRenderer(App::currentRendererType);

            App::restartRenderer = false;
        }

        if (App::lastRendererType != App::currentRendererType) {
            ShutDownCurrentRenderer();

            CreateCurrentRenderer(App::currentRendererType);
        }

        App::lastRendererType = App::currentRendererType;

        App::imGui.StartNewFrame();

        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        // GUI
        { ImGui::Begin("Rutile");

            ImGui::Text("Renderer");

            if (ImGui::Button("Restart Renderer")) {
                App::restartRenderer = true;
            }

            if (ImGui::CollapsingHeader("Lights")) {
                int pointLightCount = 1;
                int directionalLightCount = 1;
                int spotLightCount = 1;
                int i = 0;
                for (auto lightType : App::bundle.lightTypes) {
                    switch (lightType) {
                        case LightType::POINT: {
                            PointLight* light = dynamic_cast<PointLight*>(App::bundle.lights[i]);
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
                            DirectionalLight* light = dynamic_cast<DirectionalLight*>(App::bundle.lights[i]);
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
                            SpotLight* light = dynamic_cast<SpotLight*>(App::bundle.lights[i]);
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
                for (auto packet : App::bundle.packets) {
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

            if (ImGui::CollapsingHeader("Transforms")) {
                int transformCount = 1;
                int i = 0;
                for (auto t : App::bundle.transforms) {
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

        } ImGui::End();

        // Rendering
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)App::screenWidth / (float)App::screenHeight, 0.1f, 100.0f);
        std::vector<Pixel> pixels = App::renderer->Render(camera, projection);

        App::imGui.FinishFrame();

        glfwSwapBuffers(App::window);

        // Timing the frame
        {
            auto frameEnd = std::chrono::system_clock::now();

            App::frameTime = frameEnd - frameStart;
        }
    }

    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);

    App::glfw.Cleanup();
}