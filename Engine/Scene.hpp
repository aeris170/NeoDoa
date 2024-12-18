#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "System.hpp"
#include "Entity.hpp"
#include "Registry.hpp"
#include "Color.hpp"

struct Scene {

    static Scene& GetLoadedScene();

    std::string Name;
    Color ClearColor{ 0.2f, 0.3f, 0.3f };

    explicit Scene(std::string_view name = "New Scene") noexcept;

    // E - Entity
    Entity CreateEntity(std::string name = "", uint32_t desiredID = EntityTo<uint32_t>(NULL_ENTT));
    void DeleteEntity(Entity entt);
    bool ContainsEntity(Entity entt) const;
    size_t EntityCount() const;
    const std::vector<Entity>& GetAllEntites() const;

    // C - Component
    template <typename Component, typename... Args>
        requires std::constructible_from<Component, Entity, Args...>
    void EmplaceComponent(Entity entity, Args&&... args) {
        _registry.emplace<Component>(entity, entity, std::forward<Args>(args)...);
    }

    template <typename Component>
        requires std::move_constructible<Component>
    void InsertComponent(Entity entity, Component&& component) {
        _registry.emplace<Component>(entity, std::forward<Component>(component));
    }

    template <typename Component>
        requires std::move_constructible<Component>
    void ReplaceComponent(Entity entity, Component&& component) {
        _registry.replace<Component>(entity, std::forward<Component>(component));
    }

    template <typename Component>
    void InsertOrReplaceComponent(Entity entity, Component&& component) {
        if (HasComponent<Component>(entity)) {
            ReplaceComponent<Component>(entity, std::forward<Component>(component));
        } else {
            EmplaceComponent<Component>(entity, std::forward<Component>(component));
        }
    }

    template <typename Component>
    void RemoveComponent(Entity entity) {
        _registry.remove<Component>(entity);
    }

    template <typename Component>
    void RemoveComponentIfExists(Entity entity) {
        if (HasComponent<Component>(entity)) {
            RemoveComponent<Component>(entity);
        }
    }

    template <typename Component>
    bool HasComponent(Entity entity) const {
        return _registry.all_of<Component>(entity);
    }

    template <typename Component>
    Component& GetComponent(Entity entity) {
        return _registry.get<Component>(entity);
    }

    template <typename Component>
    const Component& GetComponent(Entity entity) const {
        return _registry.get<Component>(entity);
    }

    Registry& GetRegistry();

    // S - System
    // :D ?

    std::string Serialize() const;
    static Scene Deserialize(const std::string& data);

    static Scene Copy(const Scene& scene);

    void ExecuteSystems(bool isPlaying, float delta);

private:
    Registry _registry;
    std::vector<Entity> _entities;
    std::vector<entt::poly<System>> _systems;

    friend struct Core;
};