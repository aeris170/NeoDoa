#include <Engine/System.hpp>

#include <Engine/BehaviourComponent.hpp>

void BehaviourSystem::Init(Registry& reg) noexcept {
    auto view = reg.view<BehaviourComponent>();
    for (auto entity : view) {
        view.get<BehaviourComponent>(entity).Init();
    }
}

void BehaviourSystem::Execute(Registry& reg, float deltaTime) noexcept {
    auto view = reg.view<BehaviourComponent>();
    for (auto entity : view) {
        view.get<BehaviourComponent>(entity).Execute(deltaTime);
    }
}