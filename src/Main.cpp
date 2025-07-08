#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Settings/App.h"

#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "GUI/MainGUIWindow.h"

#include "renderers/CPU-Ray-Tracing/CPURayTracing.h"
#include "renderers/GPU-Ray-Tracing/GPURayTracing.h"
#include "Renderers/Voxel-Ray-Tracing/VoxelRayTracing.h"

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

void ShutdownRenderer(std::unique_ptr<Renderer>& renderer) {
    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    renderer->Cleanup(App::window);
    renderer.reset();
}

int main() {
    App::glfw.Init();

    App::renderer = CreateRenderer(App::currentRendererType);

    // Main loop
    while (!glfwWindowShouldClose(App::window)) {
        TimeScope frameTime{ &App::timingData.frameTime };

        glfwPollEvents();
        App::eventManager.Distribute();

        MoveCamera();

        App::imGui.StartNewFrame();

        MainGuiWindow();

        App::imGui.FinishFrame();

        if (App::restartRenderer || App::currentRendererType != App::lastRendererType) {
            ShutdownRenderer(App::renderer);

            App::renderer = CreateRenderer(App::currentRendererType);

            App::restartRenderer = false;
            App::lastRendererType = App::currentRendererType;

            continue;
        }

        { // Rendering
            TimeScope renderTime{ &App::timingData.renderTime };

            App::renderer->Render();

            glfwSwapBuffers(App::window);
        }
    }

    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);

    App::glfw.Cleanup();
}