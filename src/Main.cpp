#include <memory>

#include "3rdPartySystems/GLFW.h"
#include "3rdPartySystems/Window.h"
#include "3rdPartySystems/GLEW.h"
#include "3rdPartySystems/ImGuiInstance.h"

#include "renderers/OpenGl/OpenGlRenderer.h"

#include "SceneUtility/SceneManager.h"
#include "Utility/TimeScope.h"

#include "Scene/Camera.h"

#include "Statics.h"

#include "imgui.h"

using namespace Rutile;

void MoveCamera(Camera& camera, Window& window, const glm::ivec2& mousePosition);

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
    glm::ivec2 defaultFramebufferSize{ 800, 600 };

    glm::ivec2 lastFrameViewportSize{ -1, -1 };

    Framebuffer rendererFramebuffer{ };
    rendererFramebuffer.Bind();

    Texture2D targetTexture{ defaultFramebufferSize, TextureParameters{
        TextureFormat::RGB,
        TextureStorageType::UNSIGNED_BYTE,
        TextureWrapMode::REPEAT,
        TextureFilteringMode::LINEAR
    } };

    targetTexture.Bind();

    rendererFramebuffer.AddTexture(targetTexture, Framebuffer::TextureUses::COLOR_0);

    Renderbuffer renderbuffer{ defaultFramebufferSize };

    rendererFramebuffer.AddRenderbuffer(renderbuffer, Framebuffer::RenderbufferUses::DEPTH_STENCIL);

    rendererFramebuffer.Check("Renderer Buffer");

    rendererFramebuffer.Unbind();
    targetTexture.Unbind();
    renderbuffer.Unbind();

    Camera camera;

    // The offset from the top left corner of the viewport to the top left corner of the window
    glm::ivec2 viewportOffset{ };

    while(window.IsOpen()) {
        //TimeScope frameTime{ &App::timingData.frameTime }; // TODO

        glfw.PollEvents();
        
        MoveCamera(camera, window, Statics::mousePosition);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imGui.StartNewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //MainGuiWindow();

        ImGui::Begin("Sidebar");
        ImGui::End();

        ImGui::Begin("Bottombar");
        ImGui::End();

        ImGui::Begin("Viewport");

        // Needs to be the first call after "Begin"
        glm::ivec2 newViewportSize{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

        // Render the last frame with the last frames viewport size
        ImGui::Image((ImTextureID)targetTexture.Get(), ImVec2{ (float)lastFrameViewportSize.x, (float)lastFrameViewportSize.y });

        viewportOffset = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y }; // TODO

        ImGui::End();

        renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        // After ImGui has rendered its frame, we resize the framebuffer if needed for next frame
        if (newViewportSize != lastFrameViewportSize) {
            rendererFramebuffer.Bind();

            targetTexture.Bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newViewportSize.x, newViewportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture.Get(), 0);


            renderbuffer.Bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newViewportSize.x, newViewportSize.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.Get());

            rendererFramebuffer.Unbind();
            targetTexture.Unbind();
            renderbuffer.Unbind();
        }

        // TODO backup opengl state and then restore after
        // Render the next frames image
        renderer->Render(rendererFramebuffer, newViewportSize, camera);

        lastFrameViewportSize = newViewportSize;

        window.SwapBuffers();
    }
}

void MoveCamera(Camera& camera, Window& window, const glm::ivec2& mousePosition) {
    static bool mouseDown{ false };
    static bool hasMoved{ false };
    static glm::ivec2 lastMousePosition{ };

    if (!hasMoved) {
        lastMousePosition = mousePosition;
        hasMoved = true;
    }

    bool positionChange{ false };
    bool directionChange{ false };
    //const float dt = static_cast<float>(App::timingData.frameTime.count()); // TODO
    float dt = 1.0f / 60.0f;
    const float velocity = camera.speed * dt;

    if (glfwGetKey(window.Get(), GLFW_KEY_W) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.frontVector * velocity;
    }
    if (glfwGetKey(window.Get(), GLFW_KEY_S) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.frontVector * velocity;
    }
    if (glfwGetKey(window.Get(), GLFW_KEY_D) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.rightVector * velocity;
    }
    if (glfwGetKey(window.Get(), GLFW_KEY_A) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.rightVector * velocity;
    }
    if (glfwGetKey(window.Get(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.upVector * velocity;
    }
    if (glfwGetKey(window.Get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.upVector * velocity;
    }

    if (glfwGetMouseButton(window.Get(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (mouseDown == false) {
            lastMousePosition.x = mousePosition.x;
            lastMousePosition.y = mousePosition.y;
        }

        mouseDown = true;
    }

    if (glfwGetMouseButton(window.Get(), GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        mouseDown = false;
    }

    if (mouseDown) {
        const float xDelta = (float)mousePosition.x - (float)lastMousePosition.x;
        const float yDelta = (float)mousePosition.y - (float)lastMousePosition.y;

        camera.yaw += xDelta * camera.lookSensitivity;
        camera.pitch += yDelta * camera.lookSensitivity;

        if (camera.pitch > 89.9f) {
            camera.pitch = 89.9f;
        }
        else if (camera.pitch < -89.9f) {
            camera.pitch = -89.9f;
        }

        directionChange = true;
    }

    if (mouseDown 
        //|| updateCameraVectors  // TODO add back if you can adjust camera from IMGUI
        ) {
        camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.frontVector.y = sin(glm::radians(camera.pitch));
        camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.frontVector = glm::normalize(camera.frontVector);

        camera.rightVector = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

        lastMousePosition.x = mousePosition.x;
        lastMousePosition.y = mousePosition.y;

        //App::updateCameraVectors = false;
    }
}