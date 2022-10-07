#include "BehaviourComponent.hpp"


BehaviourComponent::BehaviourComponent(std::weak_ptr<Scene> scene, Entity entity) noexcept :
    _scene(scene),
    entity(entity) {};

void BehaviourComponent::Init() {}

void BehaviourComponent::Execute(float deltaTime) {}