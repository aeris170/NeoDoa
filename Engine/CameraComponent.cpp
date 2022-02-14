#include "CameraComponent.hpp"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* Ortho Camera Below */

OrthoCameraComponent::OrthoCameraComponent(const Entity owner, OrthoCamera&& data) noexcept :
	entity(owner),
	data(std::move(data)) {
	UpdateMatrices();
}

OrthoCameraComponent::OrthoCameraComponent(const Entity owner) noexcept :
	entity(owner) {
	UpdateMatrices();
}

Entity OrthoCameraComponent::GetEntity() const { return entity; }

const OrthoCamera& OrthoCameraComponent::GetData() const { return data; }
void OrthoCameraComponent::SetData(OrthoCamera&& data) {
	this->data = std::move(data);
}

void OrthoCameraComponent::UpdateMatrices() {
	data.UpdateView();
	data.UpdateProjection();
	data.UpdateViewProjection();
}

void OrthoCameraComponent::TurnOn() { isActiveAndRendering = true; }
void OrthoCameraComponent::TurnOff() { isActiveAndRendering = false; }
bool OrthoCameraComponent::IsActiveAndRendering() const { return isActiveAndRendering; }

const FrameBuffer& OrthoCameraComponent::GetFrameBuffer() const { return frameBuffer; }
void OrthoCameraComponent::SetUpOffFrameBuffer(Resolution resolution) {
	frameBuffer = resolution;
}

/* Ortho Camera Above */
/* Perpective Camera Below */

PerspectiveCameraComponent::PerspectiveCameraComponent(const Entity owner, PerspectiveCamera&& data) noexcept :
	entity(owner),
	data(std::move(data)) {
	UpdateMatrices();
}

PerspectiveCameraComponent::PerspectiveCameraComponent(const Entity owner) noexcept :
	entity(owner) {
	UpdateMatrices();
}

Entity PerspectiveCameraComponent::GetEntity() const { return entity; }

const PerspectiveCamera& PerspectiveCameraComponent::GetData() const { return data; }
void PerspectiveCameraComponent::SetData(PerspectiveCamera&& data) {
	this->data = std::move(data);
}

void PerspectiveCameraComponent::UpdateMatrices() {
	data.UpdateView();
	data.UpdateProjection();
	data.UpdateViewProjection();
}

void PerspectiveCameraComponent::TurnOn() { isActiveAndRendering = true; }
void PerspectiveCameraComponent::TurnOff() { isActiveAndRendering = false; }
bool PerspectiveCameraComponent::IsActiveAndRendering() const { return isActiveAndRendering; }

const FrameBuffer& PerspectiveCameraComponent::GetFrameBuffer() const { return frameBuffer; }
void PerspectiveCameraComponent::SetUpOffFrameBuffer(Resolution resolution) {
	frameBuffer = resolution;
}

/* Perspective Camera Above*/