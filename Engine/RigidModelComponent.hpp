#pragma once

#include <glm/glm.hpp>

#include <Engine/UUID.hpp>
#include <Engine/ECSComponent.hpp>

struct RigidModelComponent {
    explicit RigidModelComponent(const Entity owner) noexcept;
    RigidModelComponent(const Entity owner, UUID modelAssetID) noexcept;
    ~RigidModelComponent() noexcept = default;
    RigidModelComponent(const RigidModelComponent& other) noexcept;
    RigidModelComponent(RigidModelComponent&& other) noexcept;
    RigidModelComponent& operator=(const RigidModelComponent& other) noexcept;
    RigidModelComponent& operator=(RigidModelComponent&& other) noexcept;

    Entity GetEntity() const noexcept;

    UUID& GetModelUUID() noexcept;
    const UUID& GetModelUUID() const noexcept;

private:
    Entity entity{ NULL_ENTT };
    UUID modelAssetID{ UUID::Empty() };
};

static_assert(ECSComponent<RigidModelComponent>);