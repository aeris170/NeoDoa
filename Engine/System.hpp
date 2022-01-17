#pragma once

#include "BehaviourComponent.hpp"
#include "Registry.hpp"
#include "Entity.hpp"

struct System : entt::type_list<void(Registry& reg), void(Registry&, float)> {
    template<typename Base>
    struct type : Base {
        bool inited = false;
        void Init(Registry& reg) { if (inited) return; inited = true; entt::poly_call<0>(*this, reg); }
        void Execute(Registry& reg, float deltaTime) { entt::poly_call<1>(*this, reg, deltaTime); }
    };

    template<typename Type>
    using impl = entt::value_list<&Type::Init, &Type::Execute>;
};

struct BehaviourSystem {

    void Init(Registry& reg) {
        auto view = reg.view<BehaviourComponent>();
        for (auto entity : view) {
            view.get<BehaviourComponent>(entity).Init();
        }
    }

    void Execute(Registry& reg, float deltaTime) {
        auto view = reg.view<BehaviourComponent>();
        for (auto entity : view) {
            view.get<BehaviourComponent>(entity).Execute(deltaTime);
        }
    }
};