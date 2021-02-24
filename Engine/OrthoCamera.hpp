#pragma once

#include "Camera.hpp"

struct OrthoCamera : Camera {
	glm::vec3 position{ 0, 0, 0 };
	float rotation{ 0 };
	float zoom{ 1 };

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
