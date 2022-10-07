#include "IDComponent.hpp"

IDComponent::IDComponent(Entity entity, std::string tag) noexcept :
    entity(entity),
    tag(std::move(tag)) {}

Entity IDComponent::GetEntity() const { return entity; }

std::string_view IDComponent::GetTag() const { return tag; }
std::string& IDComponent::GetTagRef() { return tag; }
void IDComponent::SetTag(std::string tag) { this->tag = std::move(tag); }

void IDComponent::SetTag(const std::string& tag) {
    this->tag = tag;
}

void IDComponent::SetTag(std::string&& tag) {
    this->tag = std::move(tag);
}
