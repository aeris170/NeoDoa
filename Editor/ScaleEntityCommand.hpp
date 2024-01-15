#pragma once

#include <glm/glm.hpp>

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct ScaleEntityCommand : GUICommand {
    explicit ScaleEntityCommand(GUI& gui, Entity entity, glm::vec3 newScale) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

    bool TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept override;

private:
    Entity entity;
    glm::vec3 newScale;
    glm::vec3 oldScale;
};