#include <Editor/GameViewport.hpp>

#include <imgui.h>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>

GameViewport::GameViewport(GUI& gui) noexcept :
    gui(gui) {}

bool GameViewport::Begin() {
    ImGui::PushID(WindowStrings::GameViewportWindowName);
    bool visible = ImGui::Begin(WindowStrings::GameViewportWindowTitleID);

    return visible;
}

void GameViewport::Render() {

}

void GameViewport::End() {
    ImGui::End();
    ImGui::PopID();
}
