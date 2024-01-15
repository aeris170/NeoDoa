#include <Editor/RenameEntityCommand.hpp>

#include <format>

#include <Engine/IDComponent.hpp>

#include <Editor/GUI.hpp>

RenameEntityCommand::RenameEntityCommand(GUI& gui, Entity entity, std::string_view newName) noexcept :
    GUICommand(gui),
    entity(entity),
    newName(newName) {}

void RenameEntityCommand::Execute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<IDComponent>(entity);
    oldName = cmp.GetTag();
    cmp.SetTag(newName);

    description = std::format("Rename Entity {} to {}", oldName, newName);
}
void RenameEntityCommand::UnExecute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<IDComponent>(entity);
    cmp.SetTag(oldName);
}
