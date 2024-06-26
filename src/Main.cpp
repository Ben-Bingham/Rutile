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

#include "rendering/Camera.h"
#include "rendering/Material.h"

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

    Camera camera;

    int lastMouseX = 0;
    int lastMouseY = 0;

    // Main loop
    while (!glfwWindowShouldClose(App::window)) {
        auto frameStart = std::chrono::system_clock::now();

        glfwPollEvents();
        // Movement
        {
            float dt = static_cast<float>(App::frameTime.count());
            float velocity = camera.speed * dt;
            if (glfwGetKey(App::window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.position += camera.frontVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.position -= camera.frontVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.position += camera.rightVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.position -= camera.rightVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.position += camera.upVector * velocity;
            }
            if (glfwGetKey(App::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.position -= camera.upVector * velocity;
            }

            if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                if (App::mouseDown == false) {
                    lastMouseX = App::mousePosition.x;
                    lastMouseY = App::mousePosition.y;
                }

                App::mouseDown = true;
            }

            if (glfwGetMouseButton(App::window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
                App::mouseDown = false;
            }

            if (App::mouseDown) {
                float xDelta = (float)App::mousePosition.x - (float)lastMouseX;
                float yDelta = (float)lastMouseY - (float)App::mousePosition.y; // reversed since y-coordinates go from bottom to top

                camera.yaw += xDelta * camera.lookSensitivity;
                camera.pitch += yDelta * camera.lookSensitivity;

                if (camera.pitch > 89.9f) {
                    camera.pitch = 89.9f;
                }
                else if (camera.pitch < -89.9f) {
                    camera.pitch = -89.9f;
                }

                camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
                camera.frontVector.y = sin(glm::radians(camera.pitch));
                camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
                camera.frontVector = glm::normalize(camera.frontVector);

                camera.rightVector = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

                lastMouseX = App::mousePosition.x;
                lastMouseY = App::mousePosition.y;
            }
        }

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

            glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)App::screenWidth / (float)App::screenHeight, 0.1f, 100.0f);
            std::vector<Pixel> pixels = App::renderer->Render(camera, projection);

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