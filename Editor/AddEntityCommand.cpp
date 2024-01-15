#include <Editor/AddEntityCommand.hpp>

#include <format>

#include <Editor/GUI.hpp>

AddEntityCommand::AddEntityCommand(GUI& gui, std::string_view desiredName, uint32_t desiredEntity) noexcept :
    GUICommand(gui),
    desiredName(desiredName),
    desiredEntity(desiredEntity) {}

void AddEntityCommand::Execute() noexcept {
    addedEntity = gui.GetOpenScene().CreateEntity(desiredName, desiredEntity);
    gui.Events.OnEntityCreated(addedEntity);

    description = std::format("Create new Entity with ID: {}", std::to_string(EntityTo<uint32_t>(addedEntity)));
}
void AddEntityCommand::UnExecute() noexcept {
    gui.GetOpenScene().DeleteEntity(addedEntity);
    gui.Events.OnEntityDeleted(addedEntity);
    desiredEntity = EntityTo<uint32_t>(addedEntity);
}
