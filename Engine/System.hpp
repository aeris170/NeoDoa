#pragma once

#include <Engine/Entity.hpp>
#include <Engine/Registry.hpp>

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

    void Init(Registry& reg) noexcept;
    void Execute(Registry& reg, float deltaTime) noexcept;
};