#include "ParentComponent.hpp"

ParentComponent::ParentComponent(const Entity owner) noexcept :
    entity(owner) {}
ParentComponent::ParentComponent(const Entity owner, std::vector<Entity>&& children) noexcept :
    entity(owner),
    children(std::move(children)) {}
ParentComponent::ParentComponent(const ParentComponent& other) noexcept {
    *this = other;
}
ParentComponent::ParentComponent(ParentComponent&& other) noexcept {
    *this = std::move(other);
}
ParentComponent& ParentComponent::operator=(const ParentComponent& other) noexcept {
    children = other.children;
    return *this;
}
ParentComponent& ParentComponent::operator=(ParentComponent&& other) noexcept {
    children = std::move(other.children);
    return *this;
}

Entity ParentComponent::GetEntity() const { return entity; }

std::vector<Entity>& ParentComponent::GetChildren() { return children; }
const std::vector<Entity>& ParentComponent::GetChildren() const { return children; }
