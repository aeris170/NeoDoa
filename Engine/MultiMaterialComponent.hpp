#pragma once

#include <vector>

#include <Engine/UUID.hpp>
#include <Engine/ECSComponent.hpp>

struct MultiMaterialComponent {

    explicit MultiMaterialComponent(Entity entity) noexcept;
    ~MultiMaterialComponent() noexcept = default;
    MultiMaterialComponent(const MultiMaterialComponent& other) noexcept;
    MultiMaterialComponent(MultiMaterialComponent&& other) noexcept;
    MultiMaterialComponent& operator=(const MultiMaterialComponent& other) noexcept;
    MultiMaterialComponent& operator=(MultiMaterialComponent&& other) noexcept;

    Entity GetEntity() const noexcept;

    std::vector<UUID>& GetMaterials() noexcept;
    const std::vector<UUID>& GetMaterials() const noexcept;

private:

    Entity entity{ NULL_ENTT };
    std::vector<UUID> materials{};
};

static_assert(ECSComponent<MultiMaterialComponent>);