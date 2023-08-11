#pragma once

#include <iostream>

#include <glm/glm.hpp>

struct Region;

struct AABB {
    glm::vec3 Min{}, Max{};

    static glm::vec4 CalcNormalizedDeviceCoordinates(const AABB& aabb, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    static Region CalcScreenSpaceCoordinates(const AABB& aabb, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, glm::vec4 viewport);

    friend std::ostream& operator<<(std::ostream& os, const AABB& aabb);
};