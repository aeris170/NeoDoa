#include "SceneViewport.hpp"

#include <imgui.h>

#include "GUI.hpp"
#include "Icons.hpp"

SceneViewport::SceneViewport(GUI* gui) noexcept :
	gui(gui) {}

void SceneViewport::Begin() {
	ImGui::PushID(gui->SCENE_VIEWPORT_TITLE);
	std::string title(WindowIcons::SCENE_VIEWPORT_WINDOW_ICON);
	title.append(gui->SCENE_VIEWPORT_TITLE);
	title.append(gui->SCENE_VIEWPORT_ID);
	ImGui::Begin(title.c_str());
}

void SceneViewport::Render() {

}

void SceneViewport::End() {
	ImGui::End();
	ImGui::PopID();
}
