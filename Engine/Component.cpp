#include "Component.hpp"

#include "ComponentDeserializer.hpp"

std::string Component::Serialize() const { return declaration; }
Component Component::Deserialize(const std::string_view data) { return DeserializeComponent(data).deserializedComponent; }

Component Component::Copy(const Component& component) {
    return Component(component);
}