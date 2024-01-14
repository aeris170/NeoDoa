#include <Editor/GUICommand.hpp>

GUICommand::GUICommand(GUI& gui) noexcept : gui(gui) {}

std::string_view GUICommand::GetDescription() const noexcept { return description; }
