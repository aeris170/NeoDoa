#include "GameViewport.hpp"

#include <imgui.h>

#include "GUI.hpp"
#include "Icons.hpp"

GameViewport::GameViewport(GUI& gui) noexcept :
	gui(gui) {}

void GameViewport::Begin() {
	GUI& gui = this->gui.get();
	ImGui::PushID(gui.GAME_VIEWPORT_TITLE);
	std::string title(WindowIcons::GAME_VIEWPORT_WINDOW_ICON);
	title.append(gui.GAME_VIEWPORT_TITLE);
	title.append(gui.GAME_VIEWPORT_ID);
	ImGui::Begin(title.c_str());
}

void GameViewport::Render() {

}

void GameViewport::End() {
	ImGui::End();
	ImGui::PopID();
}
