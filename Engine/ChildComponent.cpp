#include "ChildComponent.hpp"

ChildComponent::ChildComponent(const Entity owner) noexcept :
	entity(owner) {}

ChildComponent::ChildComponent(const Entity owner, const Entity parent) noexcept :
	entity(owner),
	parent(parent) {}

Entity ChildComponent::GetEntity() const { return entity; }

Entity ChildComponent::GetParent() const { return parent; }
void ChildComponent::SetParent(const Entity parent) {
	this->parent = parent;
}
