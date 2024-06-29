#pragma once
#include "RendererType.h"
#include "Settings.h"
#include <chrono>
#include <memory>

#include "glm/glm.hpp"

#include "Scenes/Scene.h"
#include "tools/ImGuiInstance.h"
#include "tools/GLFW.h"

#include "renderers/Renderer.h"

#include "rendering/Camera.h"

#include "Scenes/SceneManager.h"

namespace Rutile {
    struct App {
        inline static std::string name = "Rutile";

        inline static Settings settings = ShadowMapTestingSceneSettings();

        inline static int screenWidth = 1200;
        inline static int screenHeight = 800;

        inline static GLFW glfw{ };
        inline static ImGuiInstance imGui{ };
        inline static GLFWwindow* window = nullptr;

        inline static Scene scene = SceneManager::GetScene(SceneType::OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE);
        inline static SceneType currentSceneType = SceneType::OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE;
        inline static SceneType lastSceneType = currentSceneType;

        inline static std::unique_ptr<Renderer> renderer = nullptr;

        inline static RendererType currentRendererType = RendererType::OPENGL;
        inline static RendererType lastRendererType = currentRendererType;
        inline static bool restartRenderer = false;

        inline static Camera camera;
        inline static bool updateCameraVectors = false;

        inline static bool mouseDown = false;
        inline static glm::ivec2 mousePosition = { 0, 0 };
        inline static glm::ivec2 lastMousePosition = mousePosition;

        inline static std::chrono::duration<double> frameTime = std::chrono::duration<double>(0.0);
    };
}