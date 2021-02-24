#pragma once

#include "Camera.hpp"

struct PerspectiveCamera : Camera {
	glm::vec3 position{ 0, 0, 0 };
	glm::vec3 rotation{ 0, 0, 0 };
	float zoom{ 1 };

	float _fov;
	float _aspect;
	float _near;
	float _far;

	PerspectiveCamera(float fov, float aspect, float near, float far);

	void UpdateView() override;
	void UpdateProjection() override;
	void UpdateViewProjection() override;
};
