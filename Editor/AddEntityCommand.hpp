#pragma once

#include <Engine/Entity.hpp>

#include <Editor/GUICommand.hpp>

struct AddEntityCommand : GUICommand {
    explicit AddEntityCommand(GUI& gui, std::string_view desiredName = "", uint32_t desiredEntity = EntityTo<uint32_t>(NULL_ENTT)) noexcept;
    void Execute() noexcept override;
    void UnExecute() noexcept override;

private:
    std::string desiredName;
    uint32_t desiredEntity;
    Entity addedEntity;
};