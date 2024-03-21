#include <Editor/Observer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Strings.hpp>

Observer::Observer(GUI& gui) noexcept :
    gui(gui) {}

bool Observer::Begin() {
    ImGui::PushID(WindowStrings::ObserverWindowName);
    bool visible = ImGui::Begin(WindowStrings::ObserverWindowTitleID);

    return visible;
}

void Observer::Render() {
    displayTargetRenderer.Render();
}

void Observer::End() {
    ImGui::End();
    ImGui::PopID();
}
