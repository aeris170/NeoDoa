#include "SceneSettings.hpp"

#include "GUI.hpp"
#include "Icons.hpp"
#include "ComponentWidgets.hpp"

#include <Scene.hpp>

SceneSettings::SceneSettings(GUI& gui) noexcept :
    gui(gui) {}

void SceneSettings::Begin(Scene* scene) {
    GUI& gui = this->gui;
    ImGui::PushID(gui.SCENE_SETTINGS_TITLE);
    std::string title(WindowIcons::SCENE_SETTINGS_WINDOW_ICON);
    title.append(gui.SCENE_SETTINGS_TITLE);
    title.append(gui.SCENE_SETTINGS_ID);
    ImGui::Begin(title.c_str());
    ImGui::PushFont(gui.GetFont());
}

void SceneSettings::Render(Scene& scene) {
    DrawStats(scene);
    ImGui::Separator();
    DrawSettings(scene);
}

void SceneSettings::End() {
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopID();
}

void SceneSettings::DrawStats(Scene& scene) {
    GUI& gui = this->gui;
    ImGui::BeginGroup();

    auto stats = scene.GetRendererStats();
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    ImGui::Text("Vertices: %d", stats.vertices);
    ImGui::Text("Indices: %d", stats.indices);
    ImGui::Text("Editor average %.3f ms/frame (%.1f FPS)", 1000.0f / gui.IO()->Framerate, gui.IO()->Framerate);
    ImGui::ColorEdit3("Clear Color", &scene.ClearColor.r);
    ImGui::ColorEdit3("Selection Outline Color", &scene.SelectionOutlineColor.r);

    ImGui::EndGroup();
}

void SceneSettings::DrawSettings(Scene& scene) {
    GUI& gui = this->gui;
    ImGui::BeginGroup();

    ImGui::NewLine();
    ImGui::PushFont(gui.GetFontBold());
    ImGui::Text("Viewport Camera:");
    ImGui::PopFont();
    static int camSelection = scene.IsOrtho() ? 0 : scene.IsPerspective() ? 1 : -1;

    /* | is intentional, || leads to invisible Perspective radio button */
    if (ImGui::RadioButton("Ortho", &camSelection, 0) | ImGui::RadioButton("Perspective", &camSelection, 1)) {
        if (camSelection == 0) {
            scene.SwitchToOrtho();
        } else if (camSelection == 1) {
            scene.SwitchToPerspective();
        }
    }
    ImGui::NewLine();
    ImGui::Text("Properties: ");

    if (scene.IsOrtho()) {
        auto& ortho = scene.GetOrtho();
        FloatWidget("Left", ortho._left, 1);
        FloatWidget("Right", ortho._right, 1);
        FloatWidget("Bottom", ortho._bottom, 1);
        FloatWidget("Top", ortho._top, 1);
        FloatWidget("Near", ortho._near, 1);
        FloatWidget("Far", ortho._far, 1);
    } else if (scene.IsPerspective()) {
        auto& perspective = scene.GetPerspective();
        FloatWidget("FOV", perspective._fov, 1, 45, 135);
        FloatWidget("Near", perspective._near, 1);
        FloatWidget("Far", perspective._far, 1);
    }
    ImGui::NewLine();

    auto& active = scene.GetActiveCamera();
    FancyVector3Widget("Eye", active.eye);
    FancyVector3Widget("Forward", active.forward);
    FancyVector3Widget("Up", active.up);
    FloatWidget("Zoom", active.zoom);

    ImGui::EndGroup();
}
