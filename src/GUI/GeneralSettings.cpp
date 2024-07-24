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
            { "OpenGl", "CPU Ray-Tracing", "GPU Ray-Tracing" },
            (int*)&App::currentRendererType
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
                "Hollow Glass Sphere"
            }, 
            (int*)&App::currentSceneType, 
            [] { App::renderer->LoadScene(); }
        );
    }
}