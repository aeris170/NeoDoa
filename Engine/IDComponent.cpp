#include "IDComponent.hpp"

IDComponent::IDComponent(Entity entity, std::string tag) noexcept :
    entity(entity),
    tag(std::move(tag)) {}
IDComponent::IDComponent(const IDComponent& other) noexcept {
    *this = other;
}
IDComponent::IDComponent(IDComponent&& other) noexcept {
    *this = std::move(other);
}
IDComponent& IDComponent::operator=(const IDComponent& other) noexcept {
    tag = other.tag;
    return *this;
}
IDComponent& IDComponent::operator=(IDComponent&& other) noexcept {
    tag = std::move(other.tag);
    return *this;
}

Entity IDComponent::GetEntity() const { return entity; }

std::string_view IDComponent::GetTag() const { return tag; }
std::string& IDComponent::GetTagRef() { return tag; }
void IDComponent::SetTag(std::string tag) { this->tag = std::move(tag); }
void IDComponent::SetTag(std::string&& tag) {
    this->tag = std::move(tag);
}
