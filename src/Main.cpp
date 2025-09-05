#include <memory>

#include "3rdPartySystems/GLFW.h"
#include "3rdPartySystems/Window.h"
#include "3rdPartySystems/GLEW.h"
#include "3rdPartySystems/ImGuiInstance.h"

#include "renderers/OpenGl/OpenGlRenderer.h"

#include "SceneUtility/SceneManager.h"
#include "Utility/TimeScope.h"

#include "Scene/Camera.h"

#include "imgui.h"

using namespace Rutile;

int main() {
    GLFW glfw{ };
    Window window{ glm::ivec2{ 1600, 900 } };
    GLEW glew;

    glfw.InitializeCallbacks(window);
    glfw.InitializeOpenGLDebug();

    ImGuiInstance imGui{ window };

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();

    Scene scene = SceneManager::GetScene(SceneType::TRIANGLE_SCENE);
    renderer->SetScene(scene);

    // Create framebuffer that renderers render to
    glm::ivec2 fbSize{ 800, 600 };

    Framebuffer framebuffer{ };
    framebuffer.Bind();

    Texture2D targetTexture{ fbSize, TextureParameters{
        TextureFormat::RGB,
        TextureStorageType::UNSIGNED_BYTE,
        TextureWrapMode::REPEAT,
        TextureFilteringMode::LINEAR
    } };

    targetTexture.Bind();

    framebuffer.AddTexture(targetTexture, Framebuffer::TextureUses::COLOR_0);

    Renderbuffer renderbuffer{ fbSize };

    framebuffer.AddRenderbuffer(renderbuffer, Framebuffer::RenderbufferUses::DEPTH_STENCIL);

    framebuffer.Check("Renderer Buffer");

    framebuffer.Unbind();
    targetTexture.Unbind();
    renderbuffer.Unbind();

    Camera camera;

    while(window.IsOpen()) {
        //TimeScope frameTime{ &App::timingData.frameTime }; // TODO

        glfw.PollEvents();
        
        MoveCamera();

        // TODO backup opengl state and then restore after
        renderer->Render(framebuffer);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imGui.StartNewFrame();

        //MainGuiWindow();

        ImGui::Begin("Viewport");

        ImGui::Image((ImTextureID)targetTexture.Get(), ImVec2{ 800.0f, 600.0f });

        ImGui::End();

        renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        window.SwapBuffers();
    }
}