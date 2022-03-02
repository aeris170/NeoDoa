#pragma once

#include "ACamera.hpp"

struct OrthoCamera : ACamera {
	float _left;
	float _right;
	float _bottom;
	float _top;
	float _near;
	float _far;

	OrthoCamera(float left, float right, float bottom, float top, float near, float far);

	void Set(float left, float right, float bottom, float top, float near, float far);
	void UpdateView() override;
	void UpdateProjection() override;
	void UpdateViewProjection() override;
};
