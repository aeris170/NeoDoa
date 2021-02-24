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
	glm::mat4 m(1); // identity
	m = glm::scale(m, { zoom, zoom, 1 });
	m *= glm::toMat4(glm::quat({ 0, 0, rotation }));
	m = glm::translate(m, -position);
	_viewMatrix = m;
}

void OrthoCamera::UpdateProjection() {
	_projectionMatrix = glm::ortho(_left, _right, _bottom, _top, _near, _far);
}

void OrthoCamera::UpdateViewProjection() {
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}