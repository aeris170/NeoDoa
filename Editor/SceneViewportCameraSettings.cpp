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
        FloatWidget(nameof(OrthoCamera::LeftPlane),   ortho.LeftPlane, 1);
        FloatWidget(nameof(OrthoCamera::RightPlane),  ortho.RightPlane, 1);
        FloatWidget(nameof(OrthoCamera::BottomPlane), ortho.BottomPlane, 1);
        FloatWidget(nameof(OrthoCamera::TopPlane),    ortho.TopPlane, 1);
        FloatWidget(nameof(OrthoCamera::NearPlane),   ortho.NearPlane, 1);
        FloatWidget(nameof(OrthoCamera::FarPlane),    ortho.FarPlane, 1);
    } else if (viewportCamera.IsPerspective()) {
        auto& perspective = viewportCamera.GetPerspectiveCamera();
        FloatWidget(nameof(PerspectiveCamera::FOV),       perspective.FOV, 1, 45, 135);
        FloatWidget(nameof(PerspectiveCamera::NearPlane), perspective.NearPlane, 1);
        FloatWidget(nameof(PerspectiveCamera::FarPlane),  perspective.FarPlane, 1);
    }
    ImGui::NewLine();

    auto& active = viewportCamera.GetActiveCamera();
    FancyVectorWidgetSettings<Display::XYZ> settings = defaultFancyVectorSettingsXYZ;
    settings.resetAllToSame = false;
    FancyVector3Widget(nameof(ACamera::Eye), active.Eye);
    settings.resetTos = { 0, 0, -1 };
    FancyVector3Widget(nameof(ACamera::Forward), active.Forward, settings);
    settings.resetTos = { 0, 1, 0 };
    FancyVector3Widget(nameof(ACamera::Up), active.Up, settings);
    FloatWidget(nameof(ACamera::Zoom), active.Zoom);

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
