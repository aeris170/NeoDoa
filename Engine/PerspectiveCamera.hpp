#pragma once

#include "Camera.hpp"

struct PerspectiveCamera : Camera {
	float _fov;
	float _aspect;
	float _near;
	float _far;

	PerspectiveCamera(float fov, float aspect, float near, float far);

	void UpdateView() override;
	void UpdateProjection() override;
	void UpdateViewProjection() override;
};
