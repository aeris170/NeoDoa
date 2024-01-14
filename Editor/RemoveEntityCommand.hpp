#pragma once

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct RemoveEntityCommand : GUICommand {
    explicit RemoveEntityCommand(GUI& gui, Entity entity) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

private:
    Entity removedEntity;
    std::string serializedEntity;
};