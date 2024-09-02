#include <Engine/OrthoCamera.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float near, float far) noexcept :
    LeftPlane(left),
    RightPlane(right),
    BottomPlane(bottom),
    TopPlane(top),
    NearPlane(near),
    FarPlane(far) {}

void OrthoCamera::Set(float left, float right, float bottom, float top, float near, float far) noexcept {
    LeftPlane = left;
    RightPlane = right;
    BottomPlane = bottom;
    TopPlane = top;
    NearPlane = near;
    FarPlane = far;
}
void OrthoCamera::UpdateView() noexcept {
    Forward = glm::normalize(Forward);
    Up = glm::normalize(Up);
    viewMatrix = glm::lookAt(Eye, Eye + Forward, Up);
    viewMatrix = glm::scale(viewMatrix, { 1, 1, 1 / Zoom });
}
void OrthoCamera::UpdateProjection() noexcept {
    projectionMatrix = glm::ortho(
        LeftPlane, RightPlane,
        BottomPlane, TopPlane,
        NearPlane, FarPlane
    );
}
void OrthoCamera::UpdateViewProjection() noexcept {
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}