#include "ParentComponent.hpp"

ParentComponent::ParentComponent(const Entity owner) noexcept :
    entity(owner) {}

ParentComponent::ParentComponent(const Entity owner, std::vector<Entity>&& children) noexcept :
    entity(owner),
    children(std::move(children)) {}

Entity ParentComponent::GetEntity() const { return entity; }

std::vector<Entity>& ParentComponent::GetChildren() { return children; }
const std::vector<Entity>& ParentComponent::GetChildren() const { return children; }
