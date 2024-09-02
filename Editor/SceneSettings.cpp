#include <Editor/SceneSettings.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/ComponentWidgets.hpp>

SceneSettings::SceneSettings(GUI& gui) noexcept :
    gui(gui) {}

bool  SceneSettings::Begin() {
    const GUI& gui = this->gui;
    ImGui::PushID(WindowStrings::SceneSettingsWindowName);
    bool visible = ImGui::Begin(WindowStrings::SceneSettingsWindowTitleID);
    ImGui::PushFont(gui.GetFont());

    return visible;
}

void SceneSettings::Render() {
    GUI& gui = this->gui;
    if (gui.HasOpenScene()) {
        DrawStats(gui.GetOpenScene());
        ImGui::Separator();
    }
}

void SceneSettings::End() {
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopID();
}

void SceneSettings::DrawStats(Scene& scene) const {
    GUI& gui = this->gui;
    ImGui::BeginGroup();

    ImGui::TextUnformatted("TODO");
    /*
    auto stats = scene.GetRendererStats();
    ImGuiFormattedText("Draw Calls: {}", stats.drawCalls);
    ImGuiFormattedText("Vertices: {}", stats.vertices);
    ImGuiFormattedText("Indices: {}", stats.indices);
    auto fps = gui.IO()->Framerate;
    ImGuiFormattedText("Editor average {:.3f} ms/frame ({:.1f} FPS)", 1000.0f / fps, fps);
    ImGui::ColorEdit3("Clear Color", &scene.ClearColor.r);
    ImGui::ColorEdit3("Selection Outline Color", &scene.SelectionOutlineColor.r);
    */

    ImGui::EndGroup();
}
