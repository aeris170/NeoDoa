#include <Engine/TransformComponent.hpp>

#include <Engine/Core.hpp>
#include <Engine/Scene.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>

#include <glm/gtx/matrix_decompose.hpp>

TransformComponent::TransformComponent(const Entity owner) noexcept :
    entity(owner) {}

glm::vec3 TransformComponent::ComputeWorldTranslation(const Entity entity, const Scene& scene) {
    glm::vec3 translation{};
    Entity currentEntity = entity;
    while (true) {
        translation += scene.GetComponent<TransformComponent>(currentEntity).GetLocalTranslation();

        if (!scene.HasComponent<ChildComponent>(currentEntity)) {
            break;
        }

        currentEntity = scene.GetComponent<ChildComponent>(currentEntity).GetParent();
    }

    return translation;
}
glm::quat TransformComponent::ComputeWorldRotation(const Entity entity, const Scene& scene) {
    glm::quat rotation{ glm::quat_identity<float, glm::packed_highp>() };
    Entity currentEntity = entity;
    while (true) {
        rotation = scene.GetComponent<TransformComponent>(currentEntity).GetLocalRotation() * rotation;

        if (!scene.HasComponent<ChildComponent>(currentEntity)) {
            break;
        }

        currentEntity = scene.GetComponent<ChildComponent>(currentEntity).GetParent();
    }

    return rotation;
}
glm::vec3 TransformComponent::ComputeLossyScale(const Entity entity, const Scene& scene) {
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
    Entity currentEntity = entity;

    while (true) {
        scale *= scene.GetComponent<TransformComponent>(currentEntity).GetLocalScale();

        if (!scene.HasComponent<ChildComponent>(currentEntity)) {
            break;
        }

        currentEntity = scene.GetComponent<ChildComponent>(currentEntity).GetParent();
    }

    return scale;
}
glm::mat4 TransformComponent::ComputeWorldMatrix(const Entity entity, const Scene& scene) {
    glm::mat4 worldMatrix{ glm::identity<glm::mat4>() };
    Entity currentEntity = entity;

    while (true) {
        const TransformComponent& transform = scene.GetComponent<TransformComponent>(currentEntity);
        glm::mat4 localMatrix = Compose(transform.GetLocalTranslation(), transform.GetLocalRotation(), transform.GetLocalScale());
        worldMatrix = localMatrix * worldMatrix;

        if (!scene.HasComponent<ChildComponent>(currentEntity)) {
            break;
        }

        currentEntity = scene.GetComponent<ChildComponent>(currentEntity).GetParent();
    }

    return worldMatrix;
}

glm::mat4 TransformComponent::Compose(glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
    glm::mat4 T = glm::translate(translation);
    glm::mat4 R = glm::toMat4(rotation);
    glm::mat4 S = glm::scale(scale);
    return T * R * S;
}
void TransformComponent::Decompose(const glm::mat4& matrix, glm::vec3* translation, glm::quat* rotation, glm::vec3* scale) {
    glm::vec3 l_translation;
    glm::quat l_rotation;
    glm::vec3 l_scale;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, l_scale, l_rotation, l_translation, skew, perspective);

    if (translation) { *translation = l_translation; }
    if (rotation)    { *rotation    = l_rotation;    }
    if (scale)       { *scale       = l_scale;       }
}

Entity TransformComponent::GetEntity() const { return entity; }

glm::vec3 TransformComponent::GetLocalTranslation() const { return localTranslation; }
void TransformComponent::SetLocalTranslation(glm::vec3 localTranslation) { this->localTranslation = localTranslation; }

glm::quat TransformComponent::GetLocalRotation() const { return localRotation; }
void TransformComponent::SetLocalRotation(glm::quat localRotation) { this->localRotation = localRotation; }

glm::vec3 TransformComponent::GetLocalScale() const { return localScale; }
void TransformComponent::SetLocalScale(glm::vec3 localScale) {    this->localScale = localScale; }