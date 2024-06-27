#include "Scenes/SceneFactory.h"
#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Settings/App.h"
#include "imgui.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "GUI/MainGUIWindow.h"

#include "rendering/Material.h"

#include "Scenes/SceneManager.h"

#include "tools/CameraMovement.h"

using namespace Rutile;

void CreateCurrentRenderer(RendererType type) {
    switch (App::currentRendererType) {
    case RendererType::OPENGL:
        App::renderer = std::make_unique<OpenGlRenderer>();
        break;
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    App::window = App::renderer->Init();
    App::renderer->SetScene(App::scene);

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

    App::scene = SceneManager::GetScene(Scenes::ORIGINAL_SCENE);

    CreateCurrentRenderer(App::currentRendererType);

    // Main loop
    while (!glfwWindowShouldClose(App::window)) {
        auto frameStart = std::chrono::system_clock::now();

        glfwPollEvents();

        MoveCamera();

        { // Reacting to settings changes
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
        }

        { // Rendering
            App::imGui.StartNewFrame();

            MainGuiWindow();

            App::renderer->Render();

            App::imGui.FinishFrame();

            glfwSwapBuffers(App::window);
        }

        // Timing the frame
        auto frameEnd = std::chrono::system_clock::now();

        App::frameTime = frameEnd - frameStart;
    }

    App::imGui.Cleanup();

    App::glfw.DetachFromWindow(App::window);

    App::renderer->Cleanup(App::window);

    App::glfw.Cleanup();
}