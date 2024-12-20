#include "ChildComponent.hpp"

ChildComponent::ChildComponent(const Entity owner) noexcept :
    entity(owner) {}
ChildComponent::ChildComponent(const Entity owner, const Entity parent) noexcept :
    entity(owner),
    parent(parent) {}
ChildComponent::ChildComponent(const ChildComponent& other) noexcept {
    *this = other;
}
ChildComponent::ChildComponent(ChildComponent&& other) noexcept {
    *this = std::move(other);
}
ChildComponent& ChildComponent::operator=(const ChildComponent& other) noexcept {
    parent = other.parent;
    return *this;
}
ChildComponent& ChildComponent::operator=(ChildComponent&& other) noexcept {
    parent = std::exchange(other.parent, NULL_ENTT);
    return *this;
}

Entity ChildComponent::GetEntity() const { return entity; }

Entity ChildComponent::GetParent() const { return parent; }
void ChildComponent::SetParent(const Entity parent) {
    this->parent = parent;
}
