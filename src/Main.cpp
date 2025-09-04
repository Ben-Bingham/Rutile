#include <memory>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

// TODO
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include "Settings/App.h"

//#include "renderers/renderer.h"

#include "Renderer.h"
#include "Scene/Scene.h"

//#include "GUI/MainGUIWindow.h"

//#include "renderers/CPU-Ray-Tracing/CPURayTracing.h"
//#include "renderers/GPU-Ray-Tracing/GPURayTracing.h"
//#include "Renderers/Voxel-Ray-Tracing/VoxelRayTracing.h"
//#include "Renderers/Software-Phong/SoftwarePhong.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "Utility/CameraMovement.h"
#include "Utility/TimeScope.h"
#include "Utility/OpenGl/GLDebug.h"

#include "3rdPartySystems/GLFW.h"
#include "3rdPartySystems/GLEW.h"
#include "3rdPartySystems/ImGuiInstance.h"

#include "imgui.h"
#include "3rdPartySystems/Window.h"

using namespace Rutile;

int main() {
    GLFW glfw;
    glfw.Init();

    Window window{ glm::ivec2{ 1600, 900 } };
    window.Init();

    GLEW glew;
    glew.Init();

    glfw.InitializeOpenGLDebug();

    ImGuiInstance imGui;
    imGui.Init(window);

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();
    renderer->Init();

    Scene scene = SceneManager::GetScene(SceneType::TRIANGLE_SCENE);

    renderer->SetScene(scene);

    //std::shared_ptr<Texture2D> rendererOutput{ };

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

    auto result = framebuffer.Check();
    if (!result) {
        std::cout << "ERROR, Framebuffer is not complete, result is: " << result << std::endl;
    }

    framebuffer.Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    renderbuffer.Unbind();

    while(window.IsOpen()) {
        TimeScope frameTime{ &App::timingData.frameTime };

        //glfw.Poll(); 
        glfwPollEvents();

        //MoveCamera(); // TODO

        // TODO backup opengl state and then restore after
        renderer->Render(framebuffer);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imGui.StartNewFrame();

        //MainGuiWindow();
        ImGui::Begin("Test");

        ImGui::Image((ImTextureID)targetTexture.Get(), ImVec2{ 800.0f, 600.0f });

        ImGui::End();

        renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        glfwSwapBuffers(window.Get());
    }

    renderer->Cleanup();
    imGui.Cleanup();
    glew.Cleanup();
    //window.Cleanup();
    glfw.Cleanup();
}