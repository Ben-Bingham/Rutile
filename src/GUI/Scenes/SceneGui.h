#pragma once
#include "rendering/Light.h"
#include "rendering/Material.h"
#include "rendering/Transform.h"

namespace Rutile {
    void MainSceneGui();

    void SceneObjects();

    void DisplayMaterial(size_t i, MaterialType type, Material* material);
    void DisplayTransform(size_t i, Transform* transform);
    void DisplayLight(size_t i, LightType type, Light* light);
}