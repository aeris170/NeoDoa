#include <Editor/SceneViewportCameraSettings.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>
#include <Editor/SceneViewport.hpp>
#include <Editor/ComponentWidgets.hpp>

SceneViewportCameraSettings::SceneViewportCameraSettings(GUI& owner) noexcept :
    gui(owner) {}

bool SceneViewportCameraSettings::Begin(ImVec2 position) noexcept {
    if (!isOpen) { return false; }

    ImGui::SetNextWindowPos(position, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ 480, 440 }, ImGuiCond_Always);
    ImGui::PushID(WindowStrings::SceneViewportCameraSettingsWindowName);
    static auto flags = ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_AlwaysAutoResize;
    bool visible = ImGui::Begin(WindowStrings::SceneViewportCameraSettingsWindowTitleID, &isOpen, flags);

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        visible = false;
        isOpen = false;
    }

    if (!isOpen) {
        isClosing = true;
    }

    return visible;
}
void SceneViewportCameraSettings::Render() noexcept {
    GUI& gui = this->gui;
    SceneViewport::ViewportCamera& viewportCamera = gui.GetSceneViewport().GetViewportCamera();
    ImGui::BeginGroup();

    ImGui::PushFont(gui.GetFontBold());
    ImGui::Text("Viewport Camera Projection:");
    ImGui::PopFont();
    int camSelection = viewportCamera.IsOrtho() ? 0 : 1;

    bool ortho = ImGui::RadioButton("Ortho", &camSelection, 0);
    bool perspective = ImGui::RadioButton("Perspective", &camSelection, 1);
    if (ortho || perspective) {
        if (camSelection == 0) {
            viewportCamera.SwitchToOrtho();
        } else if (camSelection == 1) {
            viewportCamera.SwitchToPerspective();
        }
    }
    ImGui::NewLine();
    ImGui::Text("Properties: ");

    if (viewportCamera.IsOrtho()) {
        auto& ortho = viewportCamera.GetOrthoCamera();
        FloatWidget("Left", ortho._left, 1);
        FloatWidget("Right", ortho._right, 1);
        FloatWidget("Bottom", ortho._bottom, 1);
        FloatWidget("Top", ortho._top, 1);
        FloatWidget("Near", ortho._near, 1);
        FloatWidget("Far", ortho._far, 1);
    } else if (viewportCamera.IsPerspective()) {
        auto& perspective = viewportCamera.GetPerspectiveCamera();
        FloatWidget("FOV", perspective._fov, 1, 45, 135);
        FloatWidget("Near", perspective._near, 1);
        FloatWidget("Far", perspective._far, 1);
    }
    ImGui::NewLine();

    auto& active = viewportCamera.GetActiveCamera();
    FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ;
    settings.resetAllToSame = false;
    FancyVector3Widget("Eye", active.eye);
    settings.resetTos = { 0, 0, -1 };
    FancyVector3Widget("Forward", active.forward, settings);
    settings.resetTos = { 0, 1, 0 };
    FancyVector3Widget("Up", active.up, settings);
    FloatWidget("Zoom", active.zoom);

    ImGui::EndGroup();
}
void SceneViewportCameraSettings::End() noexcept {
    if (!isOpen && !isClosing) { return; }

    isClosing = false;

    ImGui::End();
    ImGui::PopID();
}

void SceneViewportCameraSettings::Show() noexcept { isOpen = true; }
void SceneViewportCameraSettings::Hide() noexcept { isOpen = false; }
