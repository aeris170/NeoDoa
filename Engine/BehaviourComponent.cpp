#include "BehaviourComponent.hpp"


BehaviourComponent::BehaviourComponent(Scene& scene, Entity entity) noexcept :
    scene(&scene),
    entity(entity) {};
BehaviourComponent::~BehaviourComponent() {}

void BehaviourComponent::Init() {}

void BehaviourComponent::Execute([[maybe_unused]] float deltaTime) {}