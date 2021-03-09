#pragma once

#include "Camera.hpp"

struct OrthoCamera : Camera {
	float _left;
	float _right;
	float _bottom;
	float _top;
	float _near;
	float _far;

	OrthoCamera(float left, float right, float bottom, float top, float near, float far);

	void UpdateView() override;
	void UpdateProjection() override;
	void UpdateViewProjection() override;
};
