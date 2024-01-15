#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct RotateEntityCommand : GUICommand {
    explicit RotateEntityCommand(GUI& gui, Entity entity, glm::quat newRotation) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

    bool TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept override;

private:
    Entity entity;
    glm::quat newRotation;
    glm::quat oldRotation;
};