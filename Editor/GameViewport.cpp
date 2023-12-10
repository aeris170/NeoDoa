#include "GameViewport.hpp"

#include <imgui.h>

#include "GUI.hpp"
#include "Icons.hpp"

GameViewport::GameViewport(GUI& gui) noexcept :
    gui(gui) {}

bool GameViewport::Begin() {
    ImGui::PushID(GUI::GAME_VIEWPORT_TITLE);
    std::string title(WindowIcons::GAME_VIEWPORT_WINDOW_ICON);
    title.append(GUI::GAME_VIEWPORT_TITLE);
    title.append(GUI::GAME_VIEWPORT_ID);
    bool visible = ImGui::Begin(title.c_str());

    return visible;
}

void GameViewport::Render() {

}

void GameViewport::End() {
    ImGui::End();
    ImGui::PopID();
}
