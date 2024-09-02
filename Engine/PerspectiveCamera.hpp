#pragma once

#include <Engine/ACamera.hpp>

struct PerspectiveCamera : ACamera {
    float FOV;
    float AspectRatio;
    float NearPlane;
    float FarPlane;

    PerspectiveCamera(float fov, float aspect, float near, float far) noexcept;

    void Set(float fov, float aspect, float near, float far) noexcept;
    void UpdateView() noexcept override;
    void UpdateProjection() noexcept override;
    void UpdateViewProjection() noexcept override;
};