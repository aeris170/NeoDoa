#pragma once

#include "Entity.hpp"

struct ChildComponent {
    explicit ChildComponent(const Entity owner) noexcept;
    ChildComponent(const Entity owner, const Entity parent) noexcept;

    Entity GetEntity() const;

    Entity GetParent() const;
    void SetParent(const Entity parent);

private:
    Entity entity{ NULL_ENTT };
    Entity parent{ NULL_ENTT };
};