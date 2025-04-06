#pragma once

#include <vector>

#include <Engine/ECSComponent.hpp>

struct ParentComponent {
    explicit ParentComponent(const Entity owner) noexcept;
    ParentComponent(const Entity owner, std::vector<Entity>&& children) noexcept;
    ~ParentComponent() noexcept = default;
    ParentComponent(const ParentComponent& other) noexcept;
    ParentComponent(ParentComponent&& other) noexcept;
    ParentComponent& operator=(const ParentComponent& other) noexcept;
    ParentComponent& operator=(ParentComponent&& other) noexcept;

    Entity GetEntity() const;

    std::vector<Entity>& GetChildren();
    const std::vector<Entity>& GetChildren() const;

private:
    Entity entity{ NULL_ENTT };
    std::vector<Entity> children{};
};

static_assert(ECSComponent<ParentComponent>);