#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Settings/App.h"

#include "renderers/renderer.h"

#include "GUI/MainGUIWindow.h"

#include "renderers/CPU-Ray-Tracing/CPURayTracing.h"
#include "renderers/GPU-Ray-Tracing/GPURayTracing.h"
#include "Renderers/Voxel-Ray-Tracing/VoxelRayTracing.h"
#include "Renderers/Software-Phong/SoftwarePhong.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "Utility/CameraMovement.h"
#include "Utility/events/Events.h"
#include "Utility/TimeScope.h"

using namespace Rutile;

std::unique_ptr<Renderer> CreateRenderer(RendererType type) {
    std::unique_ptr<Renderer> renderer;

    switch (App::currentRendererType) {
    case RendererType::OPENGL:
        renderer = std::make_unique<OpenGlRenderer>();
        break;
    case RendererType::CPU_RAY_TRACING:
        renderer = std::make_unique<CPURayTracing>();
        break;
    case RendererType::GPU_RAY_TRACING:
        renderer = std::make_unique<GPURayTracing>();
        break;
    case RendererType::VOXEL_RAY_TRACING:
        renderer = std::make_unique<VoxelRayTracing>();
        break;
        break;
    case RendererType::SOFTWARE_PHONG:
        renderer = std::make_unique<SoftwarePhong>();
        break;
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    App::window = renderer->Init();

    for (auto object : App::scene.objects) {
        App::scene.transformBank[object.transform].CalculateMatrix();
    }

    renderer->LoadScene();
    renderer->SignalSettingsUpdate();
    renderer->ProjectionMatrixUpdate();

    App::glfw.AttachOntoWindow(App::window);

    App::imGui.Init(App::window);

    return renderer;
}

int main() {
    GLFW glfw;
    glfw.Init();

    GLEW glew;
    glew.Init();

    Window window;
    window.Init();

    ImGui imgui;
    imgui.init();

    Renderer renderer = OpenGlRenderer{ };
    renderer->Init();

    Scene scene{ DEFAULT_SCENE };

    renderer->SetScene(DEFAULT_SCENE);

    while (glfw.WindowOpen()) {
        TimeScope frameTime{ &App::timingData.frameTime };

        glfw.Poll(); // glfwPollEvents();

        MoveCamera(); // TODO

        imGui.StartNewFrame();

        MainGuiWindow();

        renderer->ProvideGui(); // TODO

        imGui.FinishFrame();

        renderer->Render();

        glfwSwapBuffers(App::window);
    }

    renderer->Cleanup();
    imGui.Cleanup();
    window.Cleanup();
    glew.Cleanup();
    glfw.Cleanup();
}