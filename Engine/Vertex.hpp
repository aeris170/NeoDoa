#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Color.hpp"

struct Vertex {
	glm::vec3 position{ glm::vec3(0, 0, 0) };
	glm::vec3 normal{ glm::vec3(0, 0, 0) };
	Color color{ 1, 1, 1, 1 };
	glm::vec2 uv{ glm::vec2(0, 0) };
	int texIndex{ -1 };
};
