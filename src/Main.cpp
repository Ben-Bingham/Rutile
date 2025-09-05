#include <memory>
#include <chrono>

#include "3rdPartySystems/GLFW.h"
#include "3rdPartySystems/Window.h"
#include "3rdPartySystems/GLEW.h"
#include "3rdPartySystems/ImGuiInstance.h"

#include "Renderers/RendererType.h"
#include "renderers/OpenGlSolidShading/OpenGlRenderer.h"

#include "SceneUtility/SceneManager.h"
#include "Utility/TimeScope.h"

#include "GUI/ImGuiUtil.h"

#include "Scene/Camera.h"

#include "Statics.h"

#include "MoveCamera.h"

#include "imgui.h"

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
    RendererType currentRendererType{ RendererType::OPENGL_SOLID_SHADING };

    // If the renderer type is changed part way through a frame, this values is updated to reflect the new type
    RendererType newRendererType{ currentRendererType };
    bool restartRenderer{ true };

    Scene scene = SceneManager::GetScene(SceneType::TRIANGLE_SCENE);
    //renderer->SetScene(scene);

    // Create framebuffer that renderers render to
    glm::ivec2 defaultFramebufferSize{ 800, 600 };
    glm::ivec2 lastFrameViewportSize{ defaultFramebufferSize };

    Framebuffer rendererFramebuffer{ };
    rendererFramebuffer.Bind();

    Texture2D targetTexture{ defaultFramebufferSize, TextureParameters{
        TextureFormat::RGB,
        TextureStorageType::UNSIGNED_BYTE,
        TextureWrapMode::REPEAT,
        TextureFilteringMode::LINEAR
    } };

    targetTexture.Bind();

    rendererFramebuffer.AddTexture(targetTexture, Framebuffer::TextureUses::COLOR_0);

    Renderbuffer renderbuffer{ defaultFramebufferSize };

    rendererFramebuffer.AddRenderbuffer(renderbuffer, Framebuffer::RenderbufferUses::DEPTH_STENCIL);

    rendererFramebuffer.Check("Renderer Buffer");

    rendererFramebuffer.Unbind();
    targetTexture.Unbind();
    renderbuffer.Unbind();

    Camera camera;

    // The offset from the top left corner of the viewport to the top left corner of the window
    glm::ivec2 viewportOffset{ };

    std::chrono::duration<double> frameTime{ };
    std::chrono::duration<double> renderTime{ };
    std::chrono::duration<double> rendererStartupTime{ };

    while (window.IsOpen()) {
        if (restartRenderer) {
            renderer.reset();

            TimeScope rendererStartupTimescope{ &rendererStartupTime };

            switch (newRendererType) {
                case RendererType::OPENGL_SOLID_SHADING: renderer = std::make_unique<OpenGlRenderer>();
            }

            renderer->SetScene(scene);

            currentRendererType = newRendererType;
            restartRenderer = false;
        }

        TimeScope frameTimeScope{ &frameTime };

        glfw.PollEvents();

        {
            TimeScope renderTimeScope{ &renderTime };

            // TODO backup opengl state and then restore after
            // Render the current frames image using the last frames viewport size
            if (renderer) renderer->Render(rendererFramebuffer, lastFrameViewportSize, camera);
        }

        glm::ivec2 mousePositionWRTViewport{ Statics::mousePosition.x - viewportOffset.x, lastFrameViewportSize.y - (viewportOffset.y - Statics::mousePosition.y) };

        MoveCamera(camera, window, static_cast<float>(frameTime.count()), mousePositionWRTViewport, lastFrameViewportSize);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imGui.StartNewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        //MainGuiWindow();

        { ImGui::Begin("Sidebar");
            if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (ImGui::Button("Restart Renderer")) {
                    restartRenderer = true;
                    newRendererType = currentRendererType;
                }

                RendererType tempRendererType{ };

                RadioButtons(
                    "Select Renderer",
                    { "OpenGl Solid Shading" },
                    (int*)&tempRendererType
                );

                if (tempRendererType != currentRendererType) {
                    restartRenderer = true;
                    newRendererType = tempRendererType;
                }
            }

            if (ImGui::CollapsingHeader("Timing Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text(std::string{ "Renderer Startup Time: " + ChronoTimeToString(rendererStartupTime) }.c_str());
                ImGui::Separator();

                ImGui::Text(std::string{ "Frame Time: " + ChronoTimeToString(frameTime) }.c_str());
                ImGui::Text(std::string{ "Render Time: " + ChronoTimeToString(renderTime) }.c_str());
            }

        } ImGui::End(); // Sidebar

        { ImGui::Begin("Bottombar");
        } ImGui::End(); // Bottombar

        glm::ivec2 newViewportSize{ };

        { ImGui::Begin("Viewport");
            // Needs to be the first call after "Begin"
            newViewportSize = glm::ivec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

            // Display the frame with the last frames viewport size (The same size it was rendered with)
            ImGui::Image((ImTextureID)targetTexture.Get(), ImVec2{ (float)lastFrameViewportSize.x, (float)lastFrameViewportSize.y });

            viewportOffset = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y }; // TODO

        } ImGui::End(); // Viewport

        if (renderer) renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        // After ImGui has rendered its frame, we resize the framebuffer if needed for next frame
        if (newViewportSize != lastFrameViewportSize) {
            rendererFramebuffer.Bind();

            targetTexture.Bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newViewportSize.x, newViewportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture.Get(), 0);

            renderbuffer.Bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newViewportSize.x, newViewportSize.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.Get());

            rendererFramebuffer.Unbind();
            targetTexture.Unbind();
            renderbuffer.Unbind();
        }

        lastFrameViewportSize = newViewportSize;

        window.SwapBuffers();
    }
}
