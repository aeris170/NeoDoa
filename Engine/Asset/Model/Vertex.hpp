#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Color.hpp"

struct Vertex {
    glm::vec3 position{ 0, 0, 0 };
    glm::vec3 normal{ 0, 0, 0 };
    Color color{ 1, 1, 1, 1 };
    glm::vec2 uv{ 0, 0 };
    uint32_t texIndex;
};
