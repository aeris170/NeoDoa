#pragma once

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct RenameEntityCommand : GUICommand {
    explicit RenameEntityCommand(GUI& gui, Entity entity, std::string_view newName) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

private:
    Entity entity;
    std::string newName;
    std::string oldName;
};