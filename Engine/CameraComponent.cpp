#include "CameraComponent.hpp"

#include <algorithm>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* Ortho Camera Below */
OrthoCameraComponent::OrthoCameraComponent(const Entity owner) noexcept :
    entity(owner) {
    UpdateMatrices();
}
OrthoCameraComponent::OrthoCameraComponent(const Entity owner, const OrthoCamera& data) noexcept :
    entity(owner),
    data(data) {
    UpdateMatrices();
}
OrthoCameraComponent::OrthoCameraComponent(const OrthoCameraComponent& other) noexcept {
    *this = other;
}
OrthoCameraComponent::OrthoCameraComponent(OrthoCameraComponent&& other) noexcept {
    *this = std::move(other);
}
OrthoCameraComponent& OrthoCameraComponent::operator=(const OrthoCameraComponent& other) noexcept {
    data = other.data;
    return *this;
}
OrthoCameraComponent& OrthoCameraComponent::operator=(OrthoCameraComponent&& other) noexcept {
    data = std::move(other.data);
    return *this;
}

Entity OrthoCameraComponent::GetEntity() const { return entity; }

const OrthoCamera& OrthoCameraComponent::GetData() const noexcept { return data; }
void OrthoCameraComponent::SetData(const OrthoCamera& data) noexcept { this->data = data; }

void OrthoCameraComponent::UpdateMatrices() {
    data.UpdateView();
    data.UpdateProjection();
    data.UpdateViewProjection();
}

void OrthoCameraComponent::TurnOn() { isActiveAndRendering = true; }
void OrthoCameraComponent::TurnOff() { isActiveAndRendering = false; }
bool OrthoCameraComponent::IsActiveAndRendering() const { return isActiveAndRendering; }
/* Ortho Camera Above */

/* Perpective Camera Below */
PerspectiveCameraComponent::PerspectiveCameraComponent(const Entity owner) noexcept :
    entity(owner) {
    UpdateMatrices();
}
PerspectiveCameraComponent::PerspectiveCameraComponent(const Entity owner, const PerspectiveCamera& data) noexcept :
    entity(owner),
    data(data) {
    UpdateMatrices();
}
PerspectiveCameraComponent::PerspectiveCameraComponent(const PerspectiveCameraComponent& other) noexcept {
    *this = other;
}
PerspectiveCameraComponent::PerspectiveCameraComponent(PerspectiveCameraComponent&& other) noexcept {
    *this = std::move(other);
}
PerspectiveCameraComponent& PerspectiveCameraComponent::operator=(const PerspectiveCameraComponent& other) noexcept {
    data = other.data;
    return *this;
}
PerspectiveCameraComponent& PerspectiveCameraComponent::operator=(PerspectiveCameraComponent&& other) noexcept {
    data = std::move(other.data);
    return *this;
}

Entity PerspectiveCameraComponent::GetEntity() const { return entity; }

const PerspectiveCamera& PerspectiveCameraComponent::GetData() const noexcept { return data; }
void PerspectiveCameraComponent::SetData(const PerspectiveCamera& data) noexcept { this->data = data; }

void PerspectiveCameraComponent::UpdateMatrices() {
    data.UpdateView();
    data.UpdateProjection();
    data.UpdateViewProjection();
}

void PerspectiveCameraComponent::TurnOn() { isActiveAndRendering = true; }
void PerspectiveCameraComponent::TurnOff() { isActiveAndRendering = false; }
bool PerspectiveCameraComponent::IsActiveAndRendering() const { return isActiveAndRendering; }
/* Perspective Camera Above*/