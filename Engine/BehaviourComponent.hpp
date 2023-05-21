#pragma once

#include <entt.hpp>

#include "Entity.hpp"
#include "Log.hpp"

struct Scene;

struct BehaviourComponent {
private:
    std::weak_ptr<Scene> _scene;

public:
    Entity entity;

    BehaviourComponent(std::weak_ptr<Scene> scene, Entity entity) noexcept;

    virtual void Init();
    virtual void Execute(float deltaTime);

    template <typename Component>
    Component& GetComponent() {
        if (!HasComponent<Component>()) {
            DOA_LOG_ERROR("Entity with ID %s does not have the requested component! Undefined behaviour!", static_cast<int>(entity));
            throw - 1;
        }
        return _scene.lock()->GetComponent<Component>(entity);
    }

    template <typename Component>
    bool TryGetComponent(Component* component) {
        if (HasComponent<Component>()) {
            component = &GetComponent<Component>();
            return true;
        }
        component = nullptr;
        return false;
    }

    template <typename Component>
    bool HasComponent() {
        return _scene.lock()->HasComponent<Component>(entity);
    }

    template <typename Component, typename... Args>
    void AddComponent(Args&&... args) {
        _scene.lock()->template AddOrReplaceComponent<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    void RemoveComponent() {
        _scene.lock()->RemoveComponentIfExists<Component>(entity);
    }
};