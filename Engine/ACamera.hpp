#pragma once

#include <glm/glm.hpp>

struct ACamera {

    virtual ~ACamera() = 0;

    virtual void UpdateView() = 0;
    virtual void UpdateProjection() = 0;
    virtual void UpdateViewProjection() = 0;

    const glm::mat4& GetViewMatrix() const noexcept;
    const glm::mat4& GetProjectionMatrix() const noexcept;
    const glm::mat4& GetViewProjectionMatrix() const noexcept;

    glm::vec3 Eye{ 0, 0, 0 };
    glm::vec3 Forward{ 0, 0, -1 };
    glm::vec3 Up{ 0, 1, 0 };
    float Zoom{ 1 };

protected:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;
};