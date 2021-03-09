#pragma once

#include <glm/glm.hpp>

struct Camera {

	glm::vec3 eye{ 0, 0, 0 };
	glm::vec3 forward{ 0, 0, -1 };
	glm::vec3 up{ 0, 1, 0 };
	float zoom{ 1 };

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewProjectionMatrix;

	virtual ~Camera() = default;

	virtual void UpdateView() = 0;
	virtual void UpdateProjection() = 0;
	virtual void UpdateViewProjection() = 0;
};