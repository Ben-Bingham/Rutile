#include "SceneGui.h"
#include "imgui.h"
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Settings/App.h"

namespace Rutile {
    void MainSceneGui() {
        const char* original =                 "Original";
        const char* triangle =                 "Triangle";
        const char* shadowMapScene =           "Shadow Map Testing";
        const char* multiLightShadowMapScene = "Multi light shadow Map Testing";
        const char* omniDirectionalShadowMapScene = "Omnidirectional shadow map test scene";

        const char* items[] = { original, triangle, shadowMapScene, multiLightShadowMapScene, omniDirectionalShadowMapScene };
        static int currentIndex = 0;

        ImGui::Text(("Current Scene: " + std::string{ items[currentIndex] }).c_str());

        if (ImGui::BeginCombo("Select a Scene", items[currentIndex])) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool isSelected = (currentIndex == n);

                if (ImGui::Selectable(items[n], isSelected)) {
                    currentIndex = n;

                    if (std::string{ items[currentIndex] } == std::string{ original }) {
                        App::currentSceneType = SceneType::ORIGINAL_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ triangle }) {
                        App::currentSceneType = SceneType::TRIANGLE_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ shadowMapScene }) {
                        App::currentSceneType = SceneType::SHADOW_MAP_TESTING_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ multiLightShadowMapScene }) {
                        App::currentSceneType = SceneType::MULTI_SHADOW_CASTER_SHADOW_MAP_TESTING_SCENE;
                    } else if (std::string{ items[currentIndex] } == std::string{ omniDirectionalShadowMapScene }) {
                        App::currentSceneType = SceneType::OMNIDIRECTIONAL_SHADOW_MAP_TESTING_SCENE;
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();
        ImGui::Text("Scene Objects");
        ImGui::Separator();

        SceneObjects();
    }

    void SceneObjects() {
        if (ImGui::TreeNode("Packets")) {
            ImGui::Separator();

            int i = 0;
            for (Packet packet : App::scene.packets) {
                ImGui::Text(("Packet #" + std::to_string(i + 1)).c_str());

                DisplayMaterial(i, packet.materialType, packet.material);
                DisplayTransform(i, packet.transform);

                ImGui::Separator();
                ++i;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lights")) {
            ImGui::Separator();

            if (App::scene.directionalLight != nullptr) {
                DisplayDirectionalLight(App::scene.directionalLight);
            }

            for (size_t i = 0; i < App::scene.lights.size(); ++i) {
                DisplayLight(i, App::scene.lightTypes[i], App::scene.lights[i]);

                ImGui::Separator();
            }

            ImGui::TreePop();
        }
    }

    void DisplayMaterial(size_t i, MaterialType type, Material* material) {
        std::string materialName{ };
        switch (type) {
            case MaterialType::SOLID: {
                materialName = "Material: Solid";
                break;
            }
            case MaterialType::PHONG: {
                materialName = "Material: Phong";
                break;
            }
        }

        if (ImGui::TreeNode((materialName + "##" + std::to_string(i)).c_str())) {
            switch (type) {
                case MaterialType::SOLID: {
                    Solid* m = dynamic_cast<Solid*>(material);

                    if (ImGui::ColorEdit3(("Color##" + std::to_string(i)).c_str(), glm::value_ptr(m->color))) { App::renderer->UpdatePacketMaterial(i); }

                    break;
                }
                case MaterialType::PHONG: {
                    Phong* m = dynamic_cast<Phong*>(material);

                    if (ImGui::ColorEdit3(("Ambient Color##"    + std::to_string(i)).c_str(), glm::value_ptr(m->ambient)))  { App::renderer->UpdatePacketMaterial(i); }
                    if (ImGui::ColorEdit3(("Diffuse Color##"    + std::to_string(i)).c_str(), glm::value_ptr(m->diffuse)))  { App::renderer->UpdatePacketMaterial(i); }
                    if (ImGui::ColorEdit3(("Specular Color##"   + std::to_string(i)).c_str(), glm::value_ptr(m->specular))) { App::renderer->UpdatePacketMaterial(i); }

                    if (ImGui::DragFloat (("Shininess##"        + std::to_string(i)).c_str(), &m->shininess, 0.5f, 0.0f, 10000.0f)) { App::renderer->UpdatePacketMaterial(i); }

                    break;
                }
            }

            ImGui::TreePop();
        }
    }

    void DisplayTransform(size_t i, Transform* transform) {
        if (ImGui::TreeNode(("Transform##" + std::to_string(i)).c_str())) {

            if (ImGui::DragFloat3(("Translation##"  + std::to_string(i)).c_str(), glm::value_ptr(transform->position),  0.01f)) { App::renderer->UpdatePacketTransform(i); }
            if (ImGui::DragFloat3(("Scale##"        + std::to_string(i)).c_str(), glm::value_ptr(transform->scale),     0.01f)) { App::renderer->UpdatePacketTransform(i); }
            if (ImGui::DragFloat4(("Rotation##"     + std::to_string(i)).c_str(), glm::value_ptr(transform->rotation),  0.01f)) { App::renderer->UpdatePacketTransform(i); }

            ImGui::TreePop();
        }
    }

    void DisplayDirectionalLight(DirectionalLight* light) {
        if (ImGui::TreeNode("Directional Light")) {
            if (ImGui::DragFloat3("Direction##Dir", glm::value_ptr(light->direction), 0.05f)) { App::renderer->UpdateSceneDirectionalLight(); }

            if (ImGui::ColorEdit3("Ambient Color##Dir", glm::value_ptr(light->ambient))) { App::renderer->UpdateSceneDirectionalLight(); }
            if (ImGui::ColorEdit3("Diffuse Color##Dir", glm::value_ptr(light->diffuse))) { App::renderer->UpdateSceneDirectionalLight(); }
            if (ImGui::ColorEdit3("Specular Color##Dir", glm::value_ptr(light->specular))) { App::renderer->UpdateSceneDirectionalLight(); }

            App::renderer->ProvideDirectionalLightVisualization();

            ImGui::TreePop();
        }
    }

    void DisplayLight(size_t i, LightType type, Light* light) {
        std::string lightName{ };
        switch (type) {
            case LightType::POINT: {
                lightName = "Point Light";
                break;
            }
            case LightType::SPOTLIGHT: {
                lightName = "Spot Light";
                break;
            }
        }

        if (ImGui::TreeNode((lightName + "##" + std::to_string(i)).c_str())) {
            switch (type) {
                case LightType::POINT: {
                    PointLight* l = dynamic_cast<PointLight*>(light);

                    if (ImGui::DragFloat3(("Position##Point"                    + std::to_string(i)).c_str(), glm::value_ptr(l->position),  0.05f)) { App::renderer->UpdateSceneLight(i); }

                    if (ImGui::DragFloat (("Constant Attenuation Component##"   + std::to_string(i)).c_str(), &l->constant,                 0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat (("Linear Attenuation Component##"     + std::to_string(i)).c_str(), &l->linear,                   0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat (("Quadratic Attenuation Component##"  + std::to_string(i)).c_str(), &l->quadratic,                0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }

                    if (ImGui::ColorEdit3(("Ambient Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->ambient))) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::ColorEdit3(("Diffuse Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->diffuse))) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::ColorEdit3(("Specular Color##"                   + std::to_string(i)).c_str(), glm::value_ptr(l->specular))) { App::renderer->UpdateSceneLight(i); }
                    break;
                }
                case LightType::SPOTLIGHT: {
                    SpotLight* l = dynamic_cast<SpotLight*>(light);

                    if (ImGui::DragFloat3(("Position##Spot"                         + std::to_string(i)).c_str(), glm::value_ptr(l->position),  0.05f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat3(("Direction##Spot"                        + std::to_string(i)).c_str(), glm::value_ptr(l->direction), 0.05f)) { App::renderer->UpdateSceneLight(i); }

                    float cutOff = glm::degrees(glm::acos(l->cutOff));
                    float outerCutOff = glm::degrees(glm::acos(l->outerCutOff));

                    if (ImGui::DragFloat (("Inner Cut Off##"                    + std::to_string(i)).c_str(), &cutOff,                      0.5f, 0.0f, 180.0f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat (("Outer Cut Off##"                    + std::to_string(i)).c_str(), &outerCutOff,                 0.5f, 0.0f, 180.0f)) { App::renderer->UpdateSceneLight(i); }

                    l->cutOff = glm::cos(glm::radians(cutOff));
                    l->outerCutOff = glm::cos(glm::radians(outerCutOff));

                    if (ImGui::DragFloat (("Constant Attenuation Component##"   + std::to_string(i)).c_str(), &l->constant,                 0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat (("Linear Attenuation Component##"     + std::to_string(i)).c_str(), &l->linear,                   0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::DragFloat (("Quadratic Attenuation Component##"  + std::to_string(i)).c_str(), &l->quadratic,                0.005f, 0.0f, 1.0f)) { App::renderer->UpdateSceneLight(i); }

                    if (ImGui::ColorEdit3(("Ambient Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->ambient))) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::ColorEdit3(("Diffuse Color##"                    + std::to_string(i)).c_str(), glm::value_ptr(l->diffuse))) { App::renderer->UpdateSceneLight(i); }
                    if (ImGui::ColorEdit3(("Specular Color##"                   + std::to_string(i)).c_str(), glm::value_ptr(l->specular))) { App::renderer->UpdateSceneLight(i); }

                    break;
                }
            }

            App::renderer->ProvideLightVisualization(i);

            ImGui::TreePop();
        }
    }
}