#pragma once
#include "RendererType.h"
#include "Settings.h"
#include <chrono>
#include <memory>

#include "glm/glm.hpp"

#include "Utility/ImGuiInstance.h"
#include "Utility/GLFW.h"

#include "renderers/Renderer.h"

#include "RenderingAPI/Camera.h"
#include "RenderingAPI/RenderingBanks.h"

#include "SceneUtility/SceneManager.h"

#include "Utility/TimingData.h"
#include "Utility/events/EventManager.h"

namespace Rutile {
    struct App {
        inline static std::string name = "Rutile";

        inline static Settings settings = ShadowMapTestingSceneSettings();

        inline static int screenWidth = 1200;
        inline static int screenHeight = 800;

        inline static GLFW glfw{ };
        inline static ImGuiInstance imGui{ };
        inline static GLFWwindow* window = nullptr;

        inline static GeometryBank geometryBank{ };
        inline static MaterialBank materialBank{ };
        inline static TransformBank transformBank{ };

        inline static Scene scene = SceneManager::GetScene(SceneType::BACKPACK);
        inline static SceneType currentSceneType = SceneType::BACKPACK;
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

        inline static TimingData timingData{ };

        inline static EventManager eventManager;

        inline static int frameCount{ 0 };
    };
}