#include <Engine/PerspectiveCamera.hpp>

#include <algorithm>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far) noexcept :
    FOV(fov),
    AspectRatio(aspect),
    NearPlane(near),
    FarPlane(far) {}

void PerspectiveCamera::Set(float fov, float aspect, float near, float far) noexcept {
    FOV = fov;
    AspectRatio = aspect;
    NearPlane = near;
    FarPlane = far;
}
void PerspectiveCamera::UpdateView() noexcept {
    Forward = glm::normalize(Forward);
    Up = glm::normalize(Up);
    viewMatrix = glm::lookAt(Eye, Eye + Forward, Up);
}
void PerspectiveCamera::UpdateProjection() noexcept {
    Zoom = std::max(1.f, Zoom);
    projectionMatrix = glm::perspective(glm::radians(FOV / Zoom), AspectRatio, NearPlane, FarPlane);
}
void PerspectiveCamera::UpdateViewProjection() noexcept {
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}
