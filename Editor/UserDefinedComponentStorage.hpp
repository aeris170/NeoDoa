#pragma once

#include <Utility/StringMap.hpp>

#include <Engine/Core.hpp>
#include <Engine/Assets.hpp>
#include <Engine/ECSComponent.hpp>

#include <Editor/ComponentInstance.hpp>

struct UserDefinedComponentStorage {

    explicit UserDefinedComponentStorage(Entity owner) noexcept;
    ~UserDefinedComponentStorage() noexcept = default;
    UserDefinedComponentStorage(const UserDefinedComponentStorage& other) noexcept;
    UserDefinedComponentStorage(UserDefinedComponentStorage&& other) noexcept;
    UserDefinedComponentStorage& operator=(const UserDefinedComponentStorage& other) noexcept;
    UserDefinedComponentStorage& operator=(UserDefinedComponentStorage&& other) noexcept;

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

static_assert(ECSComponent<UserDefinedComponentStorage>);