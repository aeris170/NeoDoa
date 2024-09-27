#pragma once

#include <vector>

#include <Engine/UUID.hpp>
#include <Engine/Entity.hpp>

struct MultiMaterialComponent {

    explicit MultiMaterialComponent(Entity entity) noexcept;

    Entity GetEntity() const noexcept;

    std::vector<UUID>& GetMaterials() noexcept;
    const std::vector<UUID>& GetMaterials() const noexcept;

private:

    Entity entity{};
    std::vector<UUID> materials{};
};