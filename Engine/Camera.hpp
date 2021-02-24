#pragma once

#include <glm/glm.hpp>

struct Camera {

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewProjectionMatrix;

	virtual ~Camera() = default;

	virtual void UpdateView() = 0;
	virtual void UpdateProjection() = 0;
	virtual void UpdateViewProjection() = 0;
};