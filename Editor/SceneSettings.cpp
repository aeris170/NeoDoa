#include "SceneSettings.hpp"

#include "GUI.hpp"
#include "Icons.hpp"

#include <Scene.hpp>

SceneSettings::SceneSettings(GUI* gui) noexcept :
	gui(gui) {}

void SceneSettings::Begin(const std::optional<Scene>& scene) {
	ImGui::PushID(gui->SCENE_SETTINGS_TITLE);
	std::string title(WindowIcons::SCENE_SETTINGS_WINDOW_ICON);
	title.append(gui->SCENE_SETTINGS_TITLE);
	title.append(gui->SCENE_SETTINGS_ID);
	ImGui::Begin(title.c_str());
}

void SceneSettings::Render(Scene& scene) {
	auto stats = scene.GetRendererStats();
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Vertices: %d", stats.vertices);
	ImGui::Text("Indices: %d", stats.indices);
	ImGui::Text("Editor average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ColorEdit3("Clear Color", &scene.ClearColor[0]);
	ImGui::ColorEdit3("Selection Outline Color", &scene.SelectionOutlineColor[0]);

	ImGui::Separator();
	static int camSelection = 1;
	if (ImGui::RadioButton("Ortho", &camSelection, 0) | ImGui::RadioButton("Perspective", &camSelection, 1)) {
		if (camSelection == 0) {
			scene.SwitchToOrtho();
		} else if (camSelection == 1) {
			scene.SwitchToPerspective();
		}
	}
	ImGui::NewLine();
	ImGui::Text("Viewport Camera: %s", camSelection == 0 ? "Ortho" : "Perspective");
	ImGui::Text("Properties: ");

	if (scene.IsOrtho()) {
		auto& ortho = scene.GetOrtho();
		ImGui::DragFloat("Left", &ortho._left);
		ImGui::DragFloat("Right", &ortho._right);
		ImGui::DragFloat("Bottom", &ortho._bottom);
		ImGui::DragFloat("Top", &ortho._top);
		ImGui::DragFloat("Near", &ortho._near);
		ImGui::DragFloat("Far", &ortho._far);
	} else if (scene.IsPerspective()) {
		auto& perspective = scene.GetPerspective();
		ImGui::DragFloat("FOV", &perspective._fov, 1, 45, 135);
		ImGui::DragFloat("Near", &perspective._near);
		ImGui::DragFloat("Far", &perspective._far);
		perspective._aspect = gui->sv.GetSize().x / gui->sv.GetSize().y;
	}
}

void SceneSettings::End() {
	ImGui::End();
	ImGui::PopID();
}
