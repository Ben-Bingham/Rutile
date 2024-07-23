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

#include "Utility/CameraMovement.h"
#include "Utility/events/Events.h"

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
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    App::window = App::renderer->Init();

    for (auto object : App::scene.objects) {
        App::transformBank[object.transform].CalculateMatrix();
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
        auto frameStartTime = std::chrono::steady_clock::now();

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

            if (App::currentSceneType != App::lastSceneType) {
                App::scene = SceneManager::GetScene(App::currentSceneType);

                for (auto object : App::scene.objects) {
                    App::transformBank[object.transform].CalculateMatrix();
                }

                App::renderer->LoadScene();
            }

            App::lastSceneType = App::currentSceneType;
        }

        { // Rendering
            App::imGui.StartNewFrame();

            auto imGuiStartTime = std::chrono::steady_clock::now();
            MainGuiWindow();
            App::timingData.imGuiTime = std::chrono::steady_clock::now() - imGuiStartTime;

            auto renderStartTime = std::chrono::steady_clock::now();
            App::renderer->Render();
            App::timingData.renderTime = std::chrono::steady_clock::now() - renderStartTime;

            App::imGui.FinishFrame();

            glfwSwapBuffers(App::window);
        }

        // Timing the frame
        App::timingData.frameTime = std::chrono::steady_clock::now() - frameStartTime;
    }

    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);

    App::glfw.Cleanup();
}