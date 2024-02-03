#include <Editor/UndoRedoHistory.hpp>

#include <cmath>

#include <imgui.h>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>
#include <Editor/GUICommand.hpp>

UndoRedoHistory::UndoRedoHistory(GUI& owner) noexcept :
    gui(owner) {}

bool UndoRedoHistory::Begin() noexcept {
    if (!isOpen) { return false; }

    ImGui::SetNextWindowSizeConstraints({ 450, 300 }, { 600, 900 });
    ImGui::PushID(WindowStrings::UndoRedoHistoryWindowName);
    bool visible = ImGui::Begin(WindowStrings::UndoRedoHistoryWindowTitleID, &isOpen);

    return visible;
}

void UndoRedoHistory::Render() noexcept {
    GUI& gui = this->gui.get();
    ImGui::BeginDisabled();
    UndoRedoStack::RedoStackIterator r_it{ gui.GetCommandHistory() };
    while(r_it.HasNext()) {
        auto [command, source] = r_it.Next();
        const GUICommand* guiCommand = dynamic_cast<GUICommand*>(&command);
        if (!guiCommand) { continue; }

        ImGui::Text(guiCommand->GetDescription().data());
    }
    ImGui::EndDisabled();

    ImGui::Separator();
    auto min = ImGui::GetItemRectMin();
    auto max = ImGui::GetItemRectMax();
    min.y -= ImGui::GetTextLineHeightWithSpacing() / 2;
    max.y += ImGui::GetTextLineHeightWithSpacing() / 2;
    if (!isMouseCaught && ImGui::IsMouseHoveringRect(min, max) && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        isMouseCaught = true;
        stackDelta = 0;
    }
    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        isMouseCaught = false;
    }
    if (isMouseCaught && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        float drag = ImGui::GetMouseDragDelta().y;
        float elemSize = ImGui::GetTextLineHeightWithSpacing();
        int newStackDelta = drag / elemSize;
        int diff = newStackDelta - stackDelta;
        while (diff > 0) { // Undo
            gui.UndoLastCommand();
            diff--;
        }
        while (diff < 0) { // Redo
            gui.RedoLastCommand();
            diff++;
        }
        stackDelta = newStackDelta;
    }


    UndoRedoStack::UndoStackReverseIterator u_it{ gui.GetCommandHistory() };
    while (u_it.HasNext()) {
        auto [command, source] = u_it.Next();
        const GUICommand* guiCommand = dynamic_cast<GUICommand*>(&command);
        if (!guiCommand) { continue; }

        ImGui::Text(guiCommand->GetDescription().data());
    }
}

void UndoRedoHistory::End() noexcept {
    ImGui::End();
    ImGui::PopID();
}

void UndoRedoHistory::Show() noexcept { isOpen = true; }
void UndoRedoHistory::Hide() noexcept { isOpen = false; }
