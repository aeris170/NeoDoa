#pragma once

#include <string>
#include <string_view>

#include <Utility/UndoRedoStack.hpp>

struct GUI;

// This struct is a virtual (abstract)
struct GUICommand : ICommand {

    explicit GUICommand(GUI& gui) noexcept;
    GUICommand(const GUICommand&) = delete;
    GUICommand(GUICommand&&) = delete;
    GUICommand& operator=(const GUICommand&) = delete;
    GUICommand& operator=(GUICommand&&) = delete;

    std::string_view GetDescription() const noexcept;

protected:
    GUI& gui;
    std::string description;
};