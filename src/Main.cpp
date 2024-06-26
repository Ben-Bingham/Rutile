#include <iostream>

#include "GeometryPreprocessor.h"
#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "renderers/HardCoded/HardCodedRenderer.h"
#include "renderers/RainbowTime/RainbowTimeRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "App.h"
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <string>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "rendering/Camera.h"
#include "rendering/Material.h"

using namespace Rutile;

void frameBufferSizeCallback(GLFWwindow* window, int w, int h) {
    App::screenWidth = w;
    App::screenHeight = h;

    if (App::renderer) {
        App::renderer->WindowResize();
    }
}

void mouseMoveCallback(GLFWwindow* window, double x, double y) {
    App::mousePosition.x = static_cast<int>(x);
    App::mousePosition.y = static_cast<int>(y);
}

enum RendererEnum {
    OPENGL          = 1,
    HARD_CODED      = 2,
    RAINBOW_TIME    = 3
};

int currentRenderer = OPENGL;

struct Settings {
    bool gpuVsync{ false };
    bool cpuVsync{ false };
};

Settings settings;

void glfwErrorCallback(int error, const char* description) {
    std::cout << "ERROR: GLFW has thrown an error: " << std::endl;
    std::cout << description << std::endl;
}

void CreateGLFWInstance() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
    }
}

void DestroyGLFWInstance() {
    glfwTerminate();
}

void AttachOntoGLFWWindow(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
}

void DetachFromGLFWWindow(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetFramebufferSizeCallback(window, nullptr);
}

int main() {
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
    Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

    //screenInit();

    CreateGLFWInstance();

    GLFWwindow* window = nullptr;

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();
    window = renderer->Init();
    //renderer->SetSize(width, height);
    renderer->SetBundle(bundle);

    AttachOntoGLFWWindow(window);

    // Init ImGui TODO USES WINDOW
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.FontGlobalScale = 2.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    

    std::chrono::duration<double> frameTime = std::chrono::duration<double>(1.0 / 60.0);

    Camera camera;

    bool mouseDown = false;

    int lastMouseX = 0;
    int lastMouseY = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        auto frameStart = std::chrono::system_clock::now();

        glfwPollEvents();
        // Movement
        {
            float dt = static_cast<float>(frameTime.count());
            float velocity = camera.speed * dt;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.position += camera.frontVector * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.position -= camera.frontVector * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.position += camera.rightVector * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.position -= camera.rightVector * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.position += camera.upVector * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.position -= camera.upVector * velocity;
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                if (mouseDown == false) {
                    lastMouseX = App::mousePosition.x;
                    lastMouseY = App::mousePosition.y;
                }

                mouseDown = true;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
                mouseDown = false;
            }

            if (mouseDown) {
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

        //spotLight.position = camera.position;
        //spotLight.direction = camera.frontVector;

        // Start ImGui frame
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        // GUI
        int lastRenderer = currentRenderer;
        { ImGui::Begin("Rutile");

            ImGui::Text("Renderer");

            ImGui::RadioButton("OpenGl",        &currentRenderer, OPENGL);          ImGui::SameLine();
            ImGui::RadioButton("Rainbow Time",  &currentRenderer, RAINBOW_TIME);    ImGui::SameLine();
            ImGui::RadioButton("Hard Coded",    &currentRenderer, HARD_CODED);

            ImGui::Text("Settings");
            ImGui::Checkbox("GPU Vsync", &settings.gpuVsync);
            ImGui::Checkbox("CPU Vsync", &settings.cpuVsync);

            if (ImGui::CollapsingHeader("Lights")) {
                int pointLightCount = 1;
                int directionalLightCount = 1;
                int spotLightCount = 1;
                int i = 0;
                for (auto lightType : bundle.lightTypes) {
                    switch (lightType) {
                        case LightType::POINT: {
                            PointLight* light = dynamic_cast<PointLight*>(bundle.lights[i]);
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
                        case LightType::DIRECTION: { // TODO
                            DirectionalLight* light = dynamic_cast<DirectionalLight*>(bundle.lights[i]);
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
                            SpotLight* light = dynamic_cast<SpotLight*>(bundle.lights[i]);
                            if (ImGui::TreeNode(("Spotlight #" + std::to_string(spotLightCount)).c_str())) {
                                ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(light->position), 0.05f);
                                ImGui::DragFloat3(("Direction##" + std::to_string(i)).c_str(), glm::value_ptr(light->direction), 0.05f);

                                float cutOff = glm::degrees(glm::acos(light->cutOff));
                                float outerCutOff = glm::degrees(glm::acos(light->outerCutOff));

                                ImGui::DragFloat(("Inner Cut Off##" + std::to_string(i)).c_str(), &cutOff, 0.5f, 0.0f, 180.0f);
                                ImGui::DragFloat(("Outer Cut Off##" + std::to_string(i)).c_str(), &outerCutOff, 0.5f, 0.0f, 180.0f);

                                /*spotLight.cutOff = glm::cos(glm::radians(cutOff));
                                spotLight.outerCutOff = glm::cos(glm::radians(outerCutOff));*/

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
                for (auto packet : bundle.packets) {
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
                for (auto t : bundle.transforms) {
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

        if (lastRenderer != currentRenderer) {
            DetachFromGLFWWindow(window);

            renderer->Cleanup(window);

            renderer.reset();

            switch (currentRenderer) {
            case OPENGL:
                renderer = std::make_unique<OpenGlRenderer>();
                break;

            case HARD_CODED:
                //renderer = std::make_unique<HardCodedRenderer>();
                break;

            case RAINBOW_TIME:
                //renderer = std::make_unique<RainbowTimeRenderer>();
                break;
            }

            renderer->Init();
            //renderer->SetSize(width, height);
            renderer->SetBundle(bundle);

            AttachOntoGLFWWindow(window);
        }

        if (settings.gpuVsync) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }

        // Rendering
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)App::screenWidth / (float)App::screenHeight, 0.1f, 100.0f);
        std::vector<Pixel> pixels = renderer->Render(camera, projection);

        // Rendering texture with pixel data
        if (!pixels.empty()) {
            //unsigned int texture;

            //glGenTextures(1, &texture);
            //glBindTexture(GL_TEXTURE_2D, texture);

            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            //glGenerateMipmap(GL_TEXTURE_2D);

            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT);

            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, texture);

            //glUseProgram(shaderProgram);
            //glBindVertexArray(VAO);
            //glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

            //glDeleteTextures(1, &texture);
        }

        // Finish ImGui frame
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* currentContextBackup = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(currentContextBackup);
            }
        }

        glfwSwapBuffers(window);

        // Timing the frame
        {
            auto frameEnd = std::chrono::system_clock::now();

            frameTime = frameEnd - frameStart;

            if (settings.cpuVsync) {
                auto elapsedTime = frameEnd - frameStart;

                auto frameDuration = std::chrono::duration<double>(1.0 / 60.0);

                auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

                if (elapsedTimeSeconds < frameDuration) {
                    auto timeToWait = frameDuration - elapsedTimeSeconds;
                    std::this_thread::sleep_for(timeToWait);
                }

                frameTime = std::chrono::system_clock::now() - frameStart;
            }
        }
    }

    // Destroy ImGui
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }

    DetachFromGLFWWindow(window);

    renderer->Cleanup(window);

    DestroyGLFWInstance();
    //screenCleanup();
}