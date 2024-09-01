#include <Editor/TranslateEntityCommand.hpp>

#include <glm/gtx/string_cast.hpp>

#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>

TranslateEntityCommand::TranslateEntityCommand(GUI& gui, Entity entity, glm::vec3 newTranslation) noexcept :
    GUICommand(gui),
    entity(entity),
    newTranslation(newTranslation),
    oldTranslation(gui.GetOpenScene().GetComponent<TransformComponent>(entity).GetLocalTranslation()) {}

void TranslateEntityCommand::Execute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalTranslation(newTranslation);

    description = std::format("Translate Entity {} to {}",
                    gui.GetOpenScene().GetComponent<IDComponent>(entity).GetTag(),
                    glm::to_string(newTranslation).substr(4)); // glm::to_string returns vec3(x,y,z) - retain after vec3
}
void TranslateEntityCommand::UnExecute() noexcept {
    auto& cmp = gui.GetOpenScene().GetComponent<TransformComponent>(entity);
    cmp.SetLocalTranslation(oldTranslation);
}

bool TranslateEntityCommand::TryMergeWith([[maybe_unused]] UndoRedoStack& history, const ICommand* command) noexcept {
    if (const TranslateEntityCommand* other = dynamic_cast<const TranslateEntityCommand*>(command)) {
        if (entity == other->entity) {
            newTranslation = other->newTranslation;
            return true;
        }
    }
    return false;
}