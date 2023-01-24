#include "UserDefinedComponentStorage.hpp"

#include <typeinfo>

UserDefinedComponentStorage::UserDefinedComponentStorage(Entity owner) noexcept :
    owner(owner) {}

Entity UserDefinedComponentStorage::Owner() const { return owner; }

unordered_string_map<ComponentInstance>& UserDefinedComponentStorage::Components() { return components; }
const unordered_string_map<ComponentInstance>& UserDefinedComponentStorage::Components() const { return components; }

ComponentInstance* UserDefinedComponentStorage::AttachComponent(Assets& assets, UUID component) {
    AssetHandle handle{ assets.FindAsset(component) };
    if (!handle->IsComponentDefinition() || !handle->HasDeserializedData()) {
        DOA_LOG_ERROR("Something went very wrong! Tried to instantiate a non-component or non-deserialized component!");
        return nullptr;
    }
    const Component& cmp = handle->DataAs<Component>();
    return &components.try_emplace(cmp.name, handle).first->second;
}

void UserDefinedComponentStorage::DetachComponent(UUID component) {
    auto search = std::ranges::find_if(components, [component](auto& elem) {
            return elem.second.ComponentAssetID() == component;
        }
    );
    if (search != components.end()) {
        components.erase(search);
    }
}
void UserDefinedComponentStorage::DetachComponent(std::string_view componentName) {
    auto search = std::ranges::find_if(components, [componentName](auto& elem) {
            return elem.first == componentName;
        }
    );
    if (search != components.end()) {
        components.erase(search);
    }
}