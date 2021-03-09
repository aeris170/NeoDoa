#include "PerspectiveCamera.hpp"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far) :
    _fov(fov),
    _aspect(aspect),
    _near(near),
    _far(far) {}

void PerspectiveCamera::UpdateView() {
    forward = glm::normalize(forward);
    up = glm::normalize(up);
    _viewMatrix = glm::lookAt(eye, eye + forward, up);
}

void PerspectiveCamera::UpdateProjection() {
    zoom = std::max(1.f, zoom);
    _projectionMatrix = glm::perspective(glm::radians(_fov / zoom), _aspect, _near, _far);
}

void PerspectiveCamera::UpdateViewProjection() {
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}
