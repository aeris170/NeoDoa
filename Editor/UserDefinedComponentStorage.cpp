#include <Editor/UserDefinedComponentStorage.hpp>

#include <typeinfo>

#include <Engine/Log.hpp>

UserDefinedComponentStorage::UserDefinedComponentStorage(Entity owner) noexcept :
    owner(owner) {}

Entity UserDefinedComponentStorage::Owner() const { return owner; }

unordered_string_map<ComponentInstance>& UserDefinedComponentStorage::Components() { return components; }
const unordered_string_map<ComponentInstance>& UserDefinedComponentStorage::Components() const { return components; }

ComponentInstance* UserDefinedComponentStorage::AttachComponent(UUID component) {
    AssetHandle handle{ Core::GetCore()->Assets()->FindAsset(component) };
    if (!handle.HasValue()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a component but definition is missing!");
        return &components.try_emplace(component.AsString(), component, InstantiationError::DEFINITION_MISSING).first->second;
    }
    if (!handle->IsComponentDefinition()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a non-component!");
        return &components.try_emplace(handle->File().Name().data(), component, InstantiationError::NON_DEFITION_INSTANTIATION).first->second;
    }
    if (!handle->HasDeserializedData()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a non-deserialized component!");
        return &components.try_emplace(handle->File().Name().data(), component, InstantiationError::DEFINITION_NOT_DESERIALIZED).first->second;
    }
    const Component& cmp = handle->DataAs<Component>();
    if (handle->HasErrorMessages()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a component with compiler errors!");
        return &components.try_emplace(cmp.name, component, InstantiationError::DEFINITION_COMPILE_ERROR).first->second;
    }
    return &components.try_emplace(cmp.name, handle).first->second;
}
ComponentInstance* UserDefinedComponentStorage::AttachComponentWithData(UUID component, std::vector<ComponentInstance::Field>&& data) {
    AssetHandle handle{ Core::GetCore()->Assets()->FindAsset(component) };
    if (!handle.HasValue()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a component but definition is missing!");
        return &components.try_emplace(component.AsString(), component, InstantiationError::DEFINITION_MISSING).first->second;
    }
    if (!handle->IsComponentDefinition()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a non-component!");
        return &components.try_emplace(handle->File().Name().data(), component, InstantiationError::NON_DEFITION_INSTANTIATION).first->second;
    }
    if (!handle->HasDeserializedData()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a non-deserialized component!");
        return &components.try_emplace(handle->File().Name().data(), component, InstantiationError::DEFINITION_NOT_DESERIALIZED).first->second;
    }
    const Component& cmp = handle->DataAs<Component>();
    if (handle->HasErrorMessages()) {
        DOA_LOG_ERROR("Something went wrong! Tried to instantiate a component with compiler errors!");
        return &components.try_emplace(cmp.name, component, InstantiationError::DEFINITION_COMPILE_ERROR).first->second;
    }
    return &components.try_emplace(cmp.name, handle, std::move(data)).first->second;
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