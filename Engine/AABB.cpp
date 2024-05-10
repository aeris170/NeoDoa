#include <Engine/AABB.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_projection.hpp>

#include <Engine/Region.hpp>

glm::vec4 AABB::CalcNormalizedDeviceCoordinates(const AABB& aabb, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 mvp = projection * view * model;
    glm::vec4 clipSpaceMin = mvp * glm::vec4(aabb.Min, 1.0f);
    glm::vec4 clipSpaceMax = mvp * glm::vec4(aabb.Max, 1.0f);

    clipSpaceMin /= clipSpaceMin.w;
    clipSpaceMax /= clipSpaceMax.w;

    return {
        clipSpaceMin.x,
        clipSpaceMin.y,
        clipSpaceMax.x,
        clipSpaceMax.y,
    };
}
Region AABB::CalcScreenSpaceCoordinates(const AABB& aabb, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, glm::vec4 viewport) {
    glm::mat mv = view * model;
    glm::vec3 p1 = glm::project({ aabb.Min.x, aabb.Min.y, aabb.Min.z }, mv, projection, viewport);
    glm::vec3 p2 = glm::project({ aabb.Max.x, aabb.Min.y, aabb.Min.z }, mv, projection, viewport);
    glm::vec3 p3 = glm::project({ aabb.Min.x, aabb.Max.y, aabb.Min.z }, mv, projection, viewport);
    glm::vec3 p4 = glm::project({ aabb.Min.x, aabb.Min.y, aabb.Max.z }, mv, projection, viewport);
    glm::vec3 p5 = glm::project({ aabb.Max.x, aabb.Max.y, aabb.Min.z }, mv, projection, viewport);
    glm::vec3 p6 = glm::project({ aabb.Max.x, aabb.Min.y, aabb.Max.z }, mv, projection, viewport);
    glm::vec3 p7 = glm::project({ aabb.Min.x, aabb.Max.y, aabb.Max.z }, mv, projection, viewport);
    glm::vec3 p8 = glm::project({ aabb.Max.x, aabb.Max.y, aabb.Max.z }, mv, projection, viewport);

    glm::vec2 min{ +std::numeric_limits<float>::max() };
    glm::vec2 max{ -std::numeric_limits<float>::max() };

    min = glm::min(
        p1,
        glm::min(p2,
        glm::min(p3,
        glm::min(p4,
        glm::min(p5,
        glm::min(p6,
        glm::min(p7,
        p8
    )))))));

    max = glm::max(
        p1,
        glm::max(p2,
        glm::max(p3,
        glm::max(p4,
        glm::max(p5,
        glm::max(p6,
        glm::max(p7,
        p8
    )))))));

    min = glm::clamp(min, glm::vec2(), glm::vec2(viewport[2] - 1, viewport[3] - 1));
    max = glm::clamp(max, glm::vec2(), glm::vec2(viewport[2] - 1, viewport[3] - 1));

    unsigned x = static_cast<unsigned>(std::min(min.x, max.x));
    unsigned y = static_cast<unsigned>(std::min(min.y, max.y));
    unsigned w = static_cast<unsigned>(std::abs(max.x - min.x));
    unsigned h = static_cast<unsigned>(std::abs(max.y - min.y));
    return { x, y, w, h };
}