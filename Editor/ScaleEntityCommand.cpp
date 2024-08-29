#include <Editor/ScaleEntityCommand.hpp>

#include <glm/gtx/string_cast.hpp>

#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>

ScaleEntityCommand::ScaleEntityCommand(GUI& gui, Entity entity, glm::vec3 newScale) noexcept :
    GUICommand(gui),
    entity(entity),
    newScale(newScale),
    oldScale(gui.GetOpenScene().GetComponent<TransformComponent>(entity).GetLocalScale()) {}

void ScaleEntityCommand::Execute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalScale(newScale);

    description = std::format("Scale Entity {} to {}",
                    gui.GetOpenScene().GetComponent<IDComponent>(entity).GetTag(),
                    glm::to_string(newScale).substr(4)); // glm::to_string returns vec3(x,y,z) - retain after vec3
}
void ScaleEntityCommand::UnExecute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalScale(oldScale);
}

bool ScaleEntityCommand::TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept {
    if (const ScaleEntityCommand* other = dynamic_cast<const ScaleEntityCommand*>(command)) {
        if (entity == other->entity) {
            newScale = other->newScale;
            return true;
        }
    }
    return false;
}