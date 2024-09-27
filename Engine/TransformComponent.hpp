#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Engine/Entity.hpp>

struct Scene;

struct TransformComponent {
    explicit TransformComponent(const Entity owner) noexcept;

    static glm::vec3 ComputeWorldTranslation(const Entity entity, const Scene& scene);
    static glm::quat ComputeWorldRotation(const Entity entity, const Scene& scene);
    static glm::vec3 ComputeLossyScale(const Entity entity, const Scene& scene);
    static glm::mat4 ComputeWorldMatrix(const Entity entity, const Scene& scene);

    static glm::mat4 Compose(glm::vec3 translation, glm::quat rotation, glm::vec3 scale);
    static void Decompose(const glm::mat4& matrix, glm::vec3* translation, glm::quat* rotation, glm::vec3* scale);

    Entity GetEntity() const;

    glm::vec3 GetLocalTranslation() const;
    void SetLocalTranslation(glm::vec3 localTranslation);

    glm::quat GetLocalRotation() const;
    void SetLocalRotation(glm::quat localRotation);

    glm::vec3 GetLocalScale() const;
    void SetLocalScale(glm::vec3 localScale);

private:
    Entity entity;

    glm::vec3 localTranslation{ 0, 0, 0 };
    glm::quat localRotation{ glm::quat_identity<float, glm::packed_highp>() };
    glm::vec3 localScale{ 1, 1, 1 };
};