#pragma once

#include <Engine/ECSComponent.hpp>

struct ChildComponent {
    explicit ChildComponent(const Entity owner) noexcept;
    ChildComponent(const Entity owner, const Entity parent) noexcept;
    ~ChildComponent() noexcept = default;
    ChildComponent(const ChildComponent& other) noexcept;
    ChildComponent(ChildComponent&& other) noexcept;
    ChildComponent& operator=(const ChildComponent& other) noexcept;
    ChildComponent& operator=(ChildComponent&& other) noexcept;

    Entity GetEntity() const;

    Entity GetParent() const;
    void SetParent(const Entity parent);

private:
    Entity entity{ NULL_ENTT };
    Entity parent{ NULL_ENTT };
};

static_assert(ECSComponent<ChildComponent>);