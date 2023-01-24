#pragma once

#include <Entity.hpp>
#include <Assets.hpp>

#include <string_map.hpp>

#include "ComponentInstance.hpp"

struct UserDefinedComponentStorage {

    explicit UserDefinedComponentStorage(Entity owner) noexcept;
    UserDefinedComponentStorage(const UserDefinedComponentStorage&) = delete;
    UserDefinedComponentStorage& operator=(const UserDefinedComponentStorage&) = delete;
    UserDefinedComponentStorage(UserDefinedComponentStorage&&) noexcept = default;
    UserDefinedComponentStorage& operator=(UserDefinedComponentStorage&&) noexcept = default;

    Entity Owner() const;

    unordered_string_map<ComponentInstance>& Components();
    const unordered_string_map<ComponentInstance>& Components() const;

    ComponentInstance* AttachComponent(Assets& assets, UUID component);

    void DetachComponent(UUID component);
    void DetachComponent(std::string_view componentName);

private:
    Entity owner;
    unordered_string_map<ComponentInstance> components;
};