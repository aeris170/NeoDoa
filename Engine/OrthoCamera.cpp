#include "OrthoCamera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float near, float far) :
	_left(left),
	_right(right),
	_bottom(bottom),
	_top(top),
	_near(near),
	_far(far) {}

void OrthoCamera::UpdateView() {
	forward = glm::normalize(forward);
	up = glm::normalize(up);
	_viewMatrix = glm::lookAt(eye, eye + forward, up);
	_viewMatrix = glm::scale(_viewMatrix, { 1, 1, 1 / zoom });
}

void OrthoCamera::UpdateProjection() {
	_projectionMatrix = glm::ortho(_left, _right, _bottom, _top, _near, _far);
}

void OrthoCamera::UpdateViewProjection() {
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}