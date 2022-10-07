#pragma once

#include <string_map.hpp>

#include "Entity.hpp"
#include "ScriptComponent.hpp"

struct ScriptComponentData;

struct ScriptStorageComponent {

    Entity _owner;
    unordered_string_map<ScriptComponent> _components;

    ScriptStorageComponent(Entity owner) noexcept;
    ScriptStorageComponent(const ScriptStorageComponent&) = delete;
    ScriptStorageComponent& operator=(const ScriptStorageComponent&) = delete;
    ScriptStorageComponent(ScriptStorageComponent&&) noexcept = default;
    ScriptStorageComponent& operator=(ScriptStorageComponent&&) noexcept = default;

    ScriptComponent& operator[](std::string_view componentType);

    bool TryGet(std::string_view componentType, ScriptComponent*& module);
    ScriptComponent& Attach(std::string_view componentType);
    ScriptComponent& AttachWithData(std::string_view componentType, const ScriptComponentData& data);
    void Detach(std::string_view componentType);
};
