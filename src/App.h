#pragma once
#include <chrono>
#include <memory>

#include "glm/glm.hpp"

namespace Rutile {
    class Renderer;

    struct App {
        inline static int screenWidth = 1200;
        inline static int screenHeight = 800;

        inline static std::unique_ptr<Renderer> renderer = nullptr;

        inline static bool mouseDown = false;
        inline static glm::ivec2 mousePosition = { 0, 0 };

        inline static std::chrono::duration<double> idealFrameTime = std::chrono::duration<double>(1.0 / 60.0);
        inline static std::chrono::duration<double> frameTime = idealFrameTime;
    };
}