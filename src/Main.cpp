#include <memory>
#include <chrono>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "3rdPartySystems/GLFW.h"
#include "3rdPartySystems/Window.h"
#include "3rdPartySystems/GLEW.h"
#include "3rdPartySystems/ImGuiInstance.h"

#include "Renderers/RendererType.h"
#include "renderers/OpenGlSolidShading/OpenGlSolidShading.h"
#include "renderers/OpenGlPhongShading/OpenGlPhongShading.h"

#include "SceneCreation/SceneManager.h"
#include "Utility/TimeScope.h"

#include "GUI/ImGuiUtil.h"

#include "Scene/Camera.h"

#include "Statics.h"

#include "MoveCamera.h"

#include "imgui.h"

#include "Utility/OpenGl/RenderTarget.h"
#include "RenderingAPI/Transform.h"

using namespace Rutile;

int main() {
    GLFW glfw{ };
    Window window{ glm::ivec2{ 1600, 900 } };
    GLEW glew;

    glfw.InitializeCallbacks(window);
    glfw.InitializeOpenGLDebug();

    ImGuiInstance imGui{ window };

    std::unique_ptr<Renderer> renderer{ };

    // The current renderer type on any given frame
    RendererType currentRendererType{ RendererType::OPENGL_PHONG_SHADING };

    // If the renderer type is changed part way through a frame, this values is updated to reflect the new type
    RendererType newRendererType{ currentRendererType };
    bool restartRenderer{ true };

    // The current scene type on any given frame
    SceneType currentSceneType{ SceneType::ORIGINAL_SCENE };
    Scene scene{ };

    // If the scene type is changed part way through a frame, this values is updated to reflect the new type
    SceneType newSceneType{ currentSceneType };
    bool resetScene{ true }; // Fully regenerate the scene, loses all changes
    bool reApplyScene{ true }; // Keep the scene data, just reload it into the renderer

    // Create framebuffer that renderers render to
    glm::ivec2 defaultFramebufferSize{ 800, 600 };
    glm::ivec2 lastFrameViewportSize{ defaultFramebufferSize };

    RenderTarget rendererTarget{ defaultFramebufferSize };

    Camera camera;

    // The offset from the top left corner of the viewport to the top left corner of the window
    glm::ivec2 viewportOffset{ };

    std::chrono::duration<double> frameTime{ };
    std::chrono::duration<double> renderTime{ };
    std::chrono::duration<double> rendererStartupTime{ };

    while (window.IsOpen()) {
        if (restartRenderer) {
            reApplyScene = true;

            renderer.reset();

            TimeScope rendererStartupTimescope{ &rendererStartupTime };

            switch (newRendererType) {
                case RendererType::OPENGL_SOLID_SHADING: renderer = std::make_unique<OpenGlSolidShading>(); break;
                case RendererType::OPENGL_PHONG_SHADING: renderer = std::make_unique<OpenGlPhongShading>(); break;
            }

            currentRendererType = newRendererType;
            restartRenderer = false;

            continue;
        }

        if (resetScene) {
            scene = SceneManager::GetScene(newSceneType);
            reApplyScene = true;

            currentSceneType = newSceneType;
            resetScene = false;

            continue;
        }

        if (reApplyScene) {
            renderer->SetScene(scene);

            reApplyScene = false;

            continue;
        }

        TimeScope frameTimeScope{ &frameTime };

        glfw.PollEvents();

        {
            TimeScope renderTimeScope{ &renderTime };

            // TODO backup opengl state and then restore after
            // Render the current frames image using the last frames viewport size
            if (renderer) renderer->Render(rendererTarget, camera);
        }

        glm::ivec2 mousePositionWRTViewport{ Statics::mousePosition.x - viewportOffset.x, lastFrameViewportSize.y - (viewportOffset.y - Statics::mousePosition.y) };

        MoveCamera(camera, window, static_cast<float>(frameTime.count()), mousePositionWRTViewport, lastFrameViewportSize);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imGui.StartNewFrame();

        ImGui::ShowDemoWindow();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        { ImGui::Begin("Sidebar");
            if (ImGui::CollapsingHeader("Timing Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text(std::string{ "Renderer Startup Time: " + ChronoTimeToString(rendererStartupTime) }.c_str());
                ImGui::Separator();

                ImGui::Text(std::string{ "Frame Time: " + ChronoTimeToString(frameTime) }.c_str());
                ImGui::Text(std::string{ "Render Time: " + ChronoTimeToString(renderTime) }.c_str());
            }

            if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (ImGui::Button("Restart Renderer")) {
                    restartRenderer = true;
                    newRendererType = currentRendererType;
                }

                RendererType tempRendererType{ currentRendererType };

                RadioButtons(
                    "Select Renderer",
                    { "OpenGl Solid Shading", "OpenGl Phong Shading"},
                    (int*)&tempRendererType
                );

                if (tempRendererType != currentRendererType) {
                    restartRenderer = true;
                    newRendererType = tempRendererType;
                }
            }

            if (ImGui::CollapsingHeader("Scene Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
                SceneType tempSceneType{ currentSceneType };

                RadioButtons(
                    "Select Scene",
                    {
                        "Hello Triangle",
                        "Original Scene",
                        //"Shadow map Testing Scene",
                        //"Omnidirectional Shadow map Testing Scene",
                        //"Double Point Light Test Scene",
                        //"All Spheres",
                        //"Spheres on Spheres",
                        //"Hollow Glass Sphere",
                        //"Ray Tracing In One Weekend",
                        //"Cornell Box",
                        //"Backpack",
                        //"Cornell Box 2.0",
                        //"8K Triangle Dragon",
                        //"80K Triangle Dragon",
                        //"800K Triangle Dragon",
                        //"Sports Car Front 3/4",
                        //"Minecraft World"
                    },
                    (int*)&tempSceneType
                );

                if (tempSceneType != currentSceneType) {
                    resetScene = true;
                    newSceneType = tempSceneType;
                }
            }

        } ImGui::End(); // Sidebar

        { ImGui::Begin("Bottombar");
            if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGuiStyle& style = ImGui::GetStyle();
                size_t objectCount = scene.objects.size();
                ImVec2 button_sz(100, 100); // TODO
                float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
                for (size_t i = 0; i < objectCount; i++) {
                    ImGui::PushID(i);

                    if (ImGui::Button(("Obj " + std::to_string(i)).c_str(), button_sz))
                        ImGui::OpenPopup("obj_popup");
                    if (ImGui::BeginPopup("obj_popup")) {
                        // TODO transform editor modes: mode 1 current, mode 2 direct edit of the matrix
                        glm::mat4 currentTransform = scene.objects[i].transform;

                        glm::vec3 scale{ };
                        glm::quat rotation{ };
                        glm::vec3 translation{ };
                        glm::vec3 skew{ };
                        glm::vec4 perspective{ };

                        bool success = glm::decompose(currentTransform, scale, rotation, translation, skew, perspective);

                        if (success) {
                            ImGui::Text("Transform");

                            bool change{ false };
                            if (ImGui::DragFloat3("Translation", glm::value_ptr(translation), 0.01f)) change = true;
                            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f)) change = true;
                            if (ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.01f)) change = true;

                            if (change) {
                                if (scale.x < 0.001) scale.x = 0.001;
                                if (scale.y < 0.001) scale.y = 0.001;
                                if (scale.z < 0.001) scale.z = 0.001;

                                Transform transform{ };
                                transform.position = translation;
                                transform.scale = scale;
                                transform.rotation = rotation;

                                transform.CalculateMatrix();

                                scene.objects[i].transform = transform.matrix;
                                renderer->UpdateObjectTransform(i, transform.matrix);
                            }
                        }
                        else {
                            ImGui::Text("Failed to decompose transform data");
                        }

                        ImGui::Text("Material");
                        // TODO add a mode switch, mode 1 is just modifying the solid colour, 2 is modifying the phong colors, and 3 could be tinting textures
                        // TODO for phong mode add a switch between settings ambient diffsue and specular seperatly and also by having ambient and specular being a percentage of diffuse
                        std::shared_ptr<Material> material = scene.objects[i].material;

                        bool change{ false };

                        if (ImGui::ColorEdit3("Ambient", glm::value_ptr(material->ambient))) change = true;
                        if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(material->diffuse))) change = true;
                        if (ImGui::ColorEdit3("Specular", glm::value_ptr(material->specular))) change = true;

                        if (ImGui::DragFloat("Shininess", &material->shininess)) change = true;

                        if (change) {
                            renderer->UpdateObjectMaterial(i, material);
                        }

                        ImGui::EndPopup();
                    }

                    float last_button_x2 = ImGui::GetItemRectMax().x;
                    float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
                    if (i + 1 < objectCount && next_button_x2 < window_visible_x2)
                        ImGui::SameLine();
                    ImGui::PopID();
                }
            }
        } ImGui::End(); // Bottombar

        glm::ivec2 newViewportSize{ };

        { ImGui::Begin("Viewport");
            // Needs to be the first call after "Begin"
            newViewportSize = glm::ivec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

            // Display the frame with the last frames viewport size (The same size it was rendered with)
            ImGui::Image((ImTextureID)rendererTarget.GetTexture().Get(), ImVec2{(float)lastFrameViewportSize.x, (float)lastFrameViewportSize.y}, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

            viewportOffset = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y }; // TODO

        } ImGui::End(); // Viewport

        if (renderer) renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        // After ImGui has rendered its frame, we resize the framebuffer if needed for next frame
        if (newViewportSize != lastFrameViewportSize) {
            rendererTarget.Resize(newViewportSize);
        }

        lastFrameViewportSize = newViewportSize;

        window.SwapBuffers();
    }
}
