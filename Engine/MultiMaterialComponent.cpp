#include <Engine/MultiMaterialComponent.hpp>

MultiMaterialComponent::MultiMaterialComponent(Entity entity) noexcept :
    entity(entity) {}

Entity MultiMaterialComponent::GetEntity() const noexcept { return entity; }

std::vector<UUID>& MultiMaterialComponent::GetMaterials() noexcept { return materials; }
const std::vector<UUID>& MultiMaterialComponent::GetMaterials() const noexcept { return materials; }