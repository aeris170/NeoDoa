#pragma once

#include <glm/glm.hpp>

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct TranslateEntityCommand : GUICommand {
    explicit TranslateEntityCommand(GUI& gui, Entity entity, glm::vec3 newTranslation) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

    bool TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept override;

private:
    Entity entity;
    glm::vec3 newTranslation;
    glm::vec3 oldTranslation;
};