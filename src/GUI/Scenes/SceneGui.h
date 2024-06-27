#pragma once
#include "rendering/Light.h"
#include "rendering/Material.h"
#include "rendering/Transform.h"

namespace Rutile {
    void MainSceneGui();

    void SceneObjects();

    void DisplayMaterial(MaterialType type, Material* material);
    void DisplayTransform(Transform* transform);
    void DisplayLight(size_t i, LightType type, Light* light);
}