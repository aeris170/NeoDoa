#include <Editor/RemoveEntityCommand.hpp>

#include <format>

#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

#include <Editor/GUI.hpp>

RemoveEntityCommand::RemoveEntityCommand(GUI& gui, Entity entity) noexcept :
    GUICommand(gui),
    removedEntity(entity) {}

void RemoveEntityCommand::Execute() noexcept {
    assert(gui.GetOpenScene().ContainsEntity(removedEntity));
    tinyxml2::XMLPrinter p;
    SceneSerializer::Entities::SerializeEntity(p, gui.GetOpenScene(), removedEntity);
    serializedEntity = p.CStr();

    gui.GetOpenScene().DeleteEntity(removedEntity);
    gui.Events.OnEntityDeleted(removedEntity);

    description = std::format("Remove Entity with ID: {}", std::to_string(EntityTo<uint32_t>(removedEntity)));
}
void RemoveEntityCommand::UnExecute() noexcept {
    assert(!gui.GetOpenScene().ContainsEntity(removedEntity));
    tinyxml2::XMLDocument d; d.Parse(serializedEntity.c_str());
    SceneDeserializer::Entities::DeserializeEntity(*d.RootElement(), gui.GetOpenScene());

    gui.Events.OnEntityCreated(removedEntity);
}
