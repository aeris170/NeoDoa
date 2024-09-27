#pragma once

#include <Engine/ACamera.hpp>

struct OrthoCamera : ACamera {
    float LeftPlane;
    float RightPlane;
    float BottomPlane;
    float TopPlane;
    float NearPlane;
    float FarPlane;

    OrthoCamera(float left, float right, float bottom, float top, float near, float far) noexcept;

    void Set(float left, float right, float bottom, float top, float near, float far) noexcept;
    void UpdateView() noexcept override;
    void UpdateProjection() noexcept override;
    void UpdateViewProjection() noexcept override;
};
