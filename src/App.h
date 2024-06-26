#pragma once
#include <memory>

#include "glm/glm.hpp"

namespace Rutile {
    class Renderer;

    struct App {
        inline static int screenWidth = 1200;
        inline static int screenHeight = 800;

        inline static std::unique_ptr<Renderer> renderer = nullptr;

        inline static glm::ivec2 mousePosition = { 0, 0 };
    };
}