#pragma once

#include <array>
#include <iostream>

#include <glm/glm.hpp>

struct Vertex {

    static constexpr int MAX_BONE_PER_VERTEX{ 4 };

    glm::vec3 Position{};
    glm::vec3 Normal{};
    glm::vec4 Color{};
    glm::vec2 TexCoords{};

    std::array<int, MAX_BONE_PER_VERTEX> BoneIDs{};
    std::array<float, MAX_BONE_PER_VERTEX> BoneWeights{};

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v);
};