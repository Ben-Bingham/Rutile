#include <memory>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Settings/App.h"

//#include "renderers/renderer.h"

#include "Renderer.h"
#include "Scene/Scene.h"

#include "GUI/MainGUIWindow.h"

#include "renderers/CPU-Ray-Tracing/CPURayTracing.h"
#include "renderers/GPU-Ray-Tracing/GPURayTracing.h"
#include "Renderers/Voxel-Ray-Tracing/VoxelRayTracing.h"
#include "Renderers/Software-Phong/SoftwarePhong.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "Utility/CameraMovement.h"
#include "Utility/events/Events.h"
#include "Utility/TimeScope.h"
#include "Utility/OpenGl/GLDebug.h"

using namespace Rutile;

//std::unique_ptr<Renderer> CreateRenderer(RendererType type) {
//    std::unique_ptr<Renderer> renderer;
//
//    switch (App::currentRendererType) {
//    case RendererType::OPENGL:
//        renderer = std::make_unique<OpenGlRenderer>();
//        break;
//    case RendererType::CPU_RAY_TRACING:
//        renderer = std::make_unique<CPURayTracing>();
//        break;
//    case RendererType::GPU_RAY_TRACING:
//        renderer = std::make_unique<GPURayTracing>();
//        break;
//    case RendererType::VOXEL_RAY_TRACING:
//        renderer = std::make_unique<VoxelRayTracing>();
//        break;
//        break;
//    case RendererType::SOFTWARE_PHONG:
//        renderer = std::make_unique<SoftwarePhong>();
//        break;
//    }
//
//    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
//    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
//
//    App::window = renderer->Init();
//
//    for (auto object : App::scene.objects) {
//        App::scene.transformBank[object.transform].CalculateMatrix();
//    }
//
//    renderer->LoadScene();
//    renderer->SignalSettingsUpdate();
//    renderer->ProjectionMatrixUpdate();
//
//    App::glfw.AttachOntoWindow(App::window);
//
//    App::imGui.Init(App::window);
//
//    return renderer;
//}

int main() {
    GLFW glfw;
    glfw.Init();

    //GLEW glew;
    //glew.Init();

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    GLFWwindow* window = glfwCreateWindow(App::screenWidth, App::screenHeight, App::name.c_str(), nullptr, nullptr);
    glfwShowWindow(window);

    if (!window) {
        std::cout << "ERROR: Failed to create window." << std::endl;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }


    //Window window;
    //window.Init();

    ImGuiInstance imgui;
    imgui.Init(window);

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();
    renderer->Init();

    Scene scene = SceneManager::GetScene(SceneType::TRIANGLE_SCENE);

    renderer->SetScene(scene);

    //while (glfw.WindowOpen()) { 
    while(glfwWindowShouldClose(window)) {
        TimeScope frameTime{ &App::timingData.frameTime };

        //glfw.Poll(); 
        glfwPollEvents();

        MoveCamera(); // TODO

        imGui.StartNewFrame();

        MainGuiWindow();

        renderer->ProvideGUI(); // TODO

        imGui.FinishFrame();

        renderer->Render();

        glfwSwapBuffers(App::window);
    }

    renderer->Cleanup();
    //imGui.Cleanup();
    //window.Cleanup();
    //glew.Cleanup();
    glfw.Cleanup();
}