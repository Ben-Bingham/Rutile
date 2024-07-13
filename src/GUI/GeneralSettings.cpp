#include "GeneralSettings.h"
#include "imgui.h"
#include "ImGuiUtil.h"

#include "Settings/App.h"

namespace Rutile {
    void GeneralSettings() {
        // Select renderer
        RadioButtons(
            "Select Renderer", 
            { "OpenGl", "CPU Ray-Tracing" },
            (int*)&App::currentRendererType,
            nullptr
        );

        if (ImGui::Button("Restart Renderer")) {
            App::restartRenderer = true;
        }

        ImGui::Separator();

        // Select material type
        RadioButtons(
            "Select Material Type", 
            { "Solid", "Phong" }, 
            (int*)&App::settings.materialType, 
            &Renderer::SignalMaterialTypeUpdate
        );

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
                "All Spheres"
            }, 
            (int*)&App::currentSceneType, 
            &Renderer::SignalNewScene
        );
    }
}