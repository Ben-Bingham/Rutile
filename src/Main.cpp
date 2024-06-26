#include "GeometryPreprocessor.h"
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
    App::renderer->SetBundle(App::bundle);

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

    GeometryPreprocessor geometryPreprocessor{ };
    // Setting up bundle
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

        Phong phong2;
        //phong2.ambient

        glm::mat4 transform1 = glm::mat4{ 1.0f };
        transform1 = glm::translate(transform1, glm::vec3{ 1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, &transform1, MaterialType::SOLID, &solid);

        glm::mat4 transform2 = glm::mat4{ 1.0f };
        transform2 = glm::translate(transform2, glm::vec3{ -1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, &transform2, MaterialType::SOLID, &solid2);

        glm::mat4 transform3 = glm::mat4{ 1.0f };
        transform3 = glm::translate(transform3, glm::vec3{ 0.0f, 0.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::SQUARE, &transform3, MaterialType::SOLID, &solid);

        glm::mat4 transform4 = glm::mat4{ 1.0f };
        transform4 = glm::translate(transform4, glm::vec3{ 1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, &transform4, MaterialType::SOLID, &solid2);

        glm::mat4 transform5 = glm::mat4{ 1.0f };
        transform5 = glm::translate(transform5, glm::vec3{ -1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, &transform5, MaterialType::PHONG, &phong);

        PointLight pointLight;
        pointLight.position = { -2.0f, 2.0f, 2.0f };

        pointLight.ambient = { 0.05f, 0.05f, 0.05f };
        pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
        pointLight.specular = { 1.0f, 1.0f, 1.0f };

        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        geometryPreprocessor.Add(LightType::POINT, &pointLight);

        DirectionalLight directionalLight;
        directionalLight.direction = { 0.0f, -1.0f, 0.0f };

        directionalLight.ambient = { 0.05f, 0.05f, 0.05f };
        directionalLight.diffuse = { 0.4f, 0.4f, 0.4f };
        directionalLight.specular = { 0.5f, 0.5f, 0.5f };

        geometryPreprocessor.Add(LightType::DIRECTION, &directionalLight);

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

        geometryPreprocessor.Add(LightType::SPOTLIGHT, &spotLight);
    }
    App::bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

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