#pragma once
#include "renderers/Renderer.h"

namespace Rutile {
    class VoxelRayTracing : public Renderer {
    public:
        GLFWwindow* Init() override;
        void Cleanup(GLFWwindow* window) override;

        void Notify(Event* event) override;
        void Render() override;
        void LoadScene() override;
    };
}