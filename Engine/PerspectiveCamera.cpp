#include "PerspectiveCamera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far) :
    _fov(fov),
    _aspect(aspect),
    _near(near),
    _far(far) {}

void PerspectiveCamera::UpdateView() {
    glm::mat4 m(1); // identity
    m = glm::scale(m, { 1, 1, 1 / zoom });
    m *= glm::toMat4(glm::quat(rotation));
    m = glm::translate(m, -position);
    _viewMatrix = m;
}

void PerspectiveCamera::UpdateProjection() {
    _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
}

void PerspectiveCamera::UpdateViewProjection() {
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}
