#include "MeshComponent.hpp"

#include "Mesh.hpp"

MeshComponent::MeshComponent(Entity entity) noexcept :
    MeshComponent(entity, {}) {}

MeshComponent::MeshComponent(Entity entity, std::weak_ptr<Mesh> mesh) noexcept :
    entity(entity),
    mesh(mesh) {}

Entity MeshComponent::GetEntity() const { return entity; }

std::weak_ptr<Mesh> MeshComponent::GetMesh() const { return mesh; }
void MeshComponent::SetMesh(std::weak_ptr<Mesh> mesh) { this->mesh = mesh; }
