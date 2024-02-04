#pragma once

#include <type_traits>

#include <entt/entt.hpp>

#include <Engine/Log.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Entity.hpp>

struct BehaviourComponent {
private:
    Scene* scene;

public:
    Entity entity;

    BehaviourComponent(Scene& scene, Entity entity) noexcept;
    virtual ~BehaviourComponent() = 0;

    virtual void Init();
    virtual void Execute(float deltaTime);

    template <typename Component>
    Component& GetComponent() {
        if (!HasComponent<Component>()) {
            DOA_LOG_ERROR("Entity with ID %s does not have the requested component! Undefined behaviour!", static_cast<int>(entity));
            throw -1;
        }
        return scene->GetComponent<Component>(entity);
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
        return scene->HasComponent<Component>(entity);
    }

    template <typename Component, typename... Args>
    void EmplaceComponent(Args&&... args) {
        scene->EmplaceComponent<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
        requires std::is_rvalue_reference_v<Component>
    void InsertComponent(Component&& component) {
        scene->InsertComponent<Component>(entity, std::move(component));
    }

    template <typename Component>
    void RemoveComponent() {
        scene->RemoveComponentIfExists<Component>(entity);
    }
};