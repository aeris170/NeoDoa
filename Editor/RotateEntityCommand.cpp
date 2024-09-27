#include <Editor/RotateEntityCommand.hpp>

#include <glm/gtx/string_cast.hpp>

#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>

RotateEntityCommand::RotateEntityCommand(GUI& gui, Entity entity, glm::quat newRotation) noexcept :
    GUICommand(gui),
    entity(entity),
    newRotation(newRotation),
    oldRotation(gui.GetOpenScene().GetComponent<TransformComponent>(entity).GetLocalRotation()) {}

void RotateEntityCommand::Execute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalRotation(newRotation);

    description = std::format("Rotate Entity {} to {}",
                    gui.GetOpenScene().GetComponent<IDComponent>(entity).GetTag(),
                    glm::to_string(glm::degrees(glm::eulerAngles(newRotation))).substr(4)); // glm::to_string returns vec3(x,y,z) - retain after vec3
}
void RotateEntityCommand::UnExecute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalRotation(oldRotation);
}

bool RotateEntityCommand::TryMergeWith([[maybe_unused]] UndoRedoStack& history, const ICommand* command) noexcept {
    if (const RotateEntityCommand* other = dynamic_cast<const RotateEntityCommand*>(command)) {
        if (entity == other->entity) {
            newRotation = other->newRotation;
            return true;
        }
    }
    return false;
}