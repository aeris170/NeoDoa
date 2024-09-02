#include <Engine/ACamera.hpp>

ACamera::~ACamera() = default;

const glm::mat4& ACamera::GetViewMatrix() const noexcept { return viewMatrix; }
const glm::mat4& ACamera::GetProjectionMatrix() const noexcept { return projectionMatrix; }
const glm::mat4& ACamera::GetViewProjectionMatrix() const noexcept { return viewProjectionMatrix; }