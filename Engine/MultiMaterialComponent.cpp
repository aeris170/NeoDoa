#include <Engine/MultiMaterialComponent.hpp>

MultiMaterialComponent::MultiMaterialComponent(Entity entity) noexcept :
    entity(entity) {}
MultiMaterialComponent::MultiMaterialComponent(const MultiMaterialComponent& other) noexcept {
    *this = other;
}
MultiMaterialComponent::MultiMaterialComponent(MultiMaterialComponent&& other) noexcept {
    *this = std::move(other);
}
MultiMaterialComponent& MultiMaterialComponent::operator=(const MultiMaterialComponent& other) noexcept {
    materials = other.materials;
    return *this;
}
MultiMaterialComponent& MultiMaterialComponent::operator=(MultiMaterialComponent&& other) noexcept {
    materials = std::move(other.materials);
    return *this;
}

Entity MultiMaterialComponent::GetEntity() const noexcept { return entity; }

std::vector<UUID>& MultiMaterialComponent::GetMaterials() noexcept { return materials; }
const std::vector<UUID>& MultiMaterialComponent::GetMaterials() const noexcept { return materials; }