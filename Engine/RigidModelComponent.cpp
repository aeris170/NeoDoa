#include <Engine/RigidModelComponent.hpp>

RigidModelComponent::RigidModelComponent(const Entity owner) noexcept :
    entity(owner) {}
RigidModelComponent::RigidModelComponent(const Entity owner, UUID modelAssetID) noexcept :
    entity(owner),
    modelAssetID(modelAssetID) {}
RigidModelComponent::RigidModelComponent(const RigidModelComponent& other) noexcept {
    *this = other;
}
RigidModelComponent::RigidModelComponent(RigidModelComponent&& other) noexcept {
    *this = std::move(other);
}
RigidModelComponent& RigidModelComponent::operator=(const RigidModelComponent& other) noexcept {
    modelAssetID = other.modelAssetID;
    return *this;
}
RigidModelComponent& RigidModelComponent::operator=(RigidModelComponent&& other) noexcept {
    modelAssetID = std::exchange(other.modelAssetID, UUID::Empty());
    return *this;
}

Entity RigidModelComponent::GetEntity() const noexcept { return entity; }

UUID& RigidModelComponent::GetModelUUID() noexcept { return modelAssetID; }
const UUID& RigidModelComponent::GetModelUUID() const noexcept { return modelAssetID; }
