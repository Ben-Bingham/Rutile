#include "SceneFactory.h"
#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "App.h"
#include "imgui.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "GUI/MainGUIWindow.h"

#include "rendering/Material.h"

#include "tools/CameraMovement.h"

using namespace Rutile;

void CreateCurrentRenderer(App::RendererType type) {
    switch (App::currentRendererType) {
    case App::RendererType::OPENGL:
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

    SceneFactory sceneFactory{ };
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

        Transform transform1{ };
        transform1.position = { 1.0f, 1.0f, 0.0f };
        sceneFactory.Add(Primitive::TRIANGLE, &transform1, MaterialType::SOLID, &solid);

        Transform transform2{ };
        transform2.position = { -1.0f, -1.0f, 0.0f };
        sceneFactory.Add(Primitive::TRIANGLE, &transform2, MaterialType::SOLID, &solid2);

        Transform transform3{ };
        transform3.position = { 0.0f, 0.0f, 0.0f };
        sceneFactory.Add(Primitive::SQUARE, &transform3, MaterialType::SOLID, &solid);

        Transform transform4{ };
        transform4.position = { 1.0f, -1.0f, 0.0f };
        sceneFactory.Add(Primitive::CUBE, &transform4, MaterialType::SOLID, &solid2);

        Transform transform5{ };
        transform5.position = { -1.0f, 1.0f, 0.0f };
        sceneFactory.Add(Primitive::CUBE, &transform5, MaterialType::PHONG, &phong);

        PointLight pointLight;
        pointLight.position = { -2.0f, 2.0f, 2.0f };

        pointLight.ambient = { 0.05f, 0.05f, 0.05f };
        pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight.specular = { 1.0f, 1.0f, 1.0f };

        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        sceneFactory.Add(LightType::POINT, &pointLight);

        DirectionalLight directionalLight;
        directionalLight.direction = { 0.0f, -1.0f, 0.0f };

        directionalLight.ambient = { 0.05f, 0.05f, 0.05f };
        directionalLight.diffuse = { 0.4f, 0.4f, 0.4f };
        directionalLight.specular = { 0.5f, 0.5f, 0.5f };

        sceneFactory.Add(LightType::DIRECTION, &directionalLight);

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

        sceneFactory.Add(LightType::SPOTLIGHT, &spotLight);
    }
    App::scene = sceneFactory.GetScene();

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