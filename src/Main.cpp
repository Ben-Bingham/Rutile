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

void CreateCurrentRenderer(RendererType type) {
    switch (App::currentRendererType) {
    case RendererType::OPENGL:
        App::renderer = std::make_unique<OpenGlRenderer>();
        break;
    case RendererType::CPU_RAY_TRACING:
        App::renderer = std::make_unique<CPURayTracing>();
        break;
    case RendererType::GPU_RAY_TRACING:
        App::renderer = std::make_unique<GPURayTracing>();
        break;
    case RendererType::VOXEL_RAY_TRACING:
        App::renderer = std::make_unique<VoxelRayTracing>();
        break;
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    App::window = App::renderer->Init();

    for (auto object : App::scene.objects) {
        App::scene.transformBank[object.transform].CalculateMatrix();
    }

    App::renderer->LoadScene();
    App::renderer->SignalSettingsUpdate();
    App::renderer->ProjectionMatrixUpdate();

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

    CreateCurrentRenderer(App::currentRendererType);

    // Main loop
    while (!glfwWindowShouldClose(App::window)) {
        TimeScope frameTime{ &App::timingData.frameTime };

        glfwPollEvents();
        App::eventManager.Distribute();

        MoveCamera();

        { // Reacting to settings changes
            if (App::restartRenderer) {
                ShutDownCurrentRenderer();

                CreateCurrentRenderer(App::currentRendererType);

                App::restartRenderer = false;
            }

            if (App::currentRendererType != App::lastRendererType) {
                ShutDownCurrentRenderer();

                CreateCurrentRenderer(App::currentRendererType);
            }

            App::lastRendererType = App::currentRendererType;
        }

        App::imGui.StartNewFrame();

        MainGuiWindow();

        if (App::currentRendererType != App::lastRendererType) {
            continue;
        }

        { // Rendering
            TimeScope renderTime{ &App::timingData.renderTime };

            App::renderer->Render();

            App::imGui.FinishFrame();

            glfwSwapBuffers(App::window);
        }
    }

    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);

    App::glfw.Cleanup();
}