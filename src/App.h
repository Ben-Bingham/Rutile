#pragma once
#include <chrono>
#include <memory>

#include "glm/glm.hpp"

#include "rendering/Bundle.h"
#include "tools/ImGuiInstance.h"
#include "tools/GLFW.h"

#include "renderers/Renderer.h"

#include "rendering/Camera.h"

namespace Rutile {
    struct App {
        static inline std::string name = "Rutile";

        inline static int screenWidth = 1200;
        inline static int screenHeight = 800;

        inline static float fieldOfView = 60.0f;
        inline static float nearPlane = 0.1f;
        inline static float farPlane = 1000.0f;

        inline static GLFW glfw{ };
        inline static ImGuiInstance imGui{ };
        inline static GLFWwindow* window = nullptr;
        inline static Bundle bundle;

        inline static std::unique_ptr<Renderer> renderer = nullptr;

        enum class RendererType {
            OPENGL
        };

        inline static RendererType currentRendererType = RendererType::OPENGL;
        inline static RendererType lastRendererType = currentRendererType;
        inline static bool restartRenderer = false;

        inline static Camera camera;

        inline static bool mouseDown = false;
        inline static glm::ivec2 mousePosition = { 0, 0 };
        inline static glm::ivec2 lastMousePosition = mousePosition;

        inline static std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);
        inline static std::chrono::duration<double> frameTime = idealFrameTime;
    };
}