#pragma once

#include <glm/glm.hpp>

#include "Color.hpp"

struct Vertex {
	glm::vec3 position{ 0, 0, 0 };
	glm::vec3 normal{ 0, 0, 0 };
	Color color{ 1, 1, 1, 1 };
	glm::vec2 uv{ 0, 0 };
	int texIndex{ -1 };
};
