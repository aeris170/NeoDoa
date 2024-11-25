#pragma once

#include <Utility/StringMap.hpp>

#include <Engine/Core.hpp>
#include <Engine/Entity.hpp>
#include <Engine/Assets.hpp>

#include <Editor/ComponentInstance.hpp>

struct UserDefinedComponentStorage {

    explicit UserDefinedComponentStorage(Entity owner) noexcept;
    UserDefinedComponentStorage(const UserDefinedComponentStorage&) = delete;
    UserDefinedComponentStorage& operator=(const UserDefinedComponentStorage&) = delete;
    UserDefinedComponentStorage(UserDefinedComponentStorage&&) noexcept = default;
    UserDefinedComponentStorage& operator=(UserDefinedComponentStorage&&) noexcept = default;

    Entity Owner() const;

    unordered_string_map<ComponentInstance>& Components();
    const unordered_string_map<ComponentInstance>& Components() const;

    ComponentInstance* AttachComponent(UUID component, Assets& assets);
    ComponentInstance* AttachComponentWithData(UUID component, Assets& assets, std::vector<ComponentInstance::Field>&& data);

    void DetachComponent(UUID component);
    void DetachComponent(std::string_view componentName);

private:
    Entity owner;
    unordered_string_map<ComponentInstance> components;
};