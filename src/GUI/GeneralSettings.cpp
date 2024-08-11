#include "GeneralSettings.h"
#include "imgui.h"
#include "ImGuiUtil.h"

#include "Settings/App.h"

#include "Utility/events/Events.h"

namespace Rutile {
    void GeneralSettings() {
        // Select renderer
        RadioButtons(
            "Select Renderer", 
            { "OpenGl", "CPU Ray-Tracing", "GPU Ray-Tracing", "Voxel Ray-Tracing" },
            (int*)&App::currentRendererType,
            [] {
                switch(App::currentRendererType) {
                case RendererType::OPENGL:
                    App::settings.materialType = MaterialType::PHONG;
                    break;

                case RendererType::CPU_RAY_TRACING:
                    App::settings.materialType = MaterialType::RAY_TRACING;
                    break;

                case RendererType::GPU_RAY_TRACING:
                    App::settings.materialType = MaterialType::RAY_TRACING;
                    break;
                case RendererType::VOXEL_RAY_TRACING:
                    App::settings.materialType = MaterialType::RAY_TRACING;
                    break;
                }

                App::eventManager.Notify(new MaterialTypeUpdate{ });
            }
        );

        if (ImGui::Button("Restart Renderer")) {
            App::restartRenderer = true;
        }

        ImGui::Separator();

        // Select material type
        if (App::currentRendererType == RendererType::OPENGL) {
            RadioButtons(
                "Select Material Type",
                { "Solid", "Phong" },
                (int*)&App::settings.materialType,
                [] { App::eventManager.Notify(new MaterialTypeUpdate{ }); }
            );
        } else {
            App::settings.materialType = MaterialType::RAY_TRACING;
            RadioButtons(
                "Select Material Type",
                { "Ray Tracing" },
                (int*)&App::settings.materialType,
                [] { App::eventManager.Notify(new MaterialTypeUpdate{ }); }
            );
        }

        ImGui::Separator();

        // Select scene
        RadioButtons(
            "Select Scene", 
            {
                "Hello Triangle",
                "Original Scene",
                "Shadow map Testing Scene",
                "Omnidirectional Shadow map Testing Scene",
                "Double Point Light Test Scene",
                "All Spheres",
                "Spheres on Spheres",
                "Hollow Glass Sphere",
                "Ray Tracing In One Weekend",
                "Cornell Box",
                "Backpack",
                "Cornell Box 2.0"
            }, 
            (int*)&App::sceneType, 
            [] {
                App::scene = SceneManager::GetScene(App::sceneType);

                for (auto object : App::scene.objects) {
                    App::scene.transformBank[object.transform].CalculateMatrix();
                }

                App::renderer->LoadScene();
            }
        );
    }
}