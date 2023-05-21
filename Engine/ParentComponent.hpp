#pragma once

#include <vector>

#include "Entity.hpp"

struct ParentComponent {
    explicit ParentComponent(const Entity owner) noexcept;
    ParentComponent(const Entity owner, std::vector<Entity>&& children) noexcept;

    Entity GetEntity() const;

    std::vector<Entity>& GetChildren();
    const std::vector<Entity>& GetChildren() const;

private:
    Entity entity{ NULL_ENTT };
    std::vector<Entity> children{};

};