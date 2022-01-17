#include "TransformComponent.hpp"

#include "Scene.hpp"
#include "ParentComponent.hpp"
#include "ChildComponent.hpp"

TransformComponent::TransformComponent(const Entity owner, std::weak_ptr<Scene> scene) noexcept :
    entity(owner),
    scene(scene) {}

TransformComponent::TransformComponent(const Entity owner, glm::mat4&& matrix) noexcept :
    entity(owner),
    localMatrix(std::move(matrix)) {
    Decompose(matrix, localTranslation, localRotation, localScale);
}

void TransformComponent::InsertTranslation(glm::mat4& m, const glm::vec3& translation) {
    m[3] = glm::vec4(translation, 1.0f);
}

void TransformComponent::InsertRotation(glm::mat4& m, const glm::quat& rotation) {
    glm::quat orig = ExtractRotation(m);
    m *= glm::toMat4(glm::inverse(orig));
    m *= glm::toMat4(rotation);
}

void TransformComponent::InsertScale(glm::mat4& m, const glm::vec3& scale) {
    glm::vec3 orig = ExtractScale(m);
    for (int i = 0; i < 3; i++) {
        orig[i] = 1 / orig[i];
    }
    m *= glm::scale(orig);
    m *= glm::scale(scale);
}

void TransformComponent::Compose(glm::mat4& m, const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale) {
    glm::mat4 T = glm::translate(translation);
    glm::mat4 R = glm::toMat4(rotation);
    glm::mat4 S = glm::scale(scale);
    m = T * R * S;
}

glm::vec3 TransformComponent::ExtractTranslation(const glm::mat4& m) {
    return glm::vec3(m[3]);
}

glm::quat TransformComponent::ExtractRotation(const glm::mat4& m) {
    glm::vec3 scale;
    for (int i = 0; i < 3; i++) {
        scale[i] = glm::length(glm::vec3(m[i]));
    }

    const glm::mat3 rotMtx(
        glm::vec3(m[0]) / scale[0],
        glm::vec3(m[1]) / scale[1],
        glm::vec3(m[2]) / scale[2]
    );

    return(glm::quat_cast(rotMtx));
}

glm::vec3 TransformComponent::ExtractScale(const glm::mat4& m) {
    glm::vec3 scale;
    for (int i = 0; i < 3; i++) {
        scale[i] = glm::length(glm::vec3(m[i]));
    }
    return scale;
}

void TransformComponent::Decompose(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale) {
    pos = ExtractTranslation(m);
    rot = ExtractRotation(m);
    scale = ExtractScale(m);
}

Entity TransformComponent::GetEntity() const { return entity; }

glm::vec3 TransformComponent::GetWorldTranslation() const { return ExtractTranslation(GetWorldMatrix()); }
void TransformComponent::SetWorldTranslation(glm::vec3 worldTranslation) {
    if (scene.expired()) return;

    std::shared_ptr<Scene> scenePtr = scene.lock();

    glm::vec3 local = worldTranslation;
    if (scenePtr->HasComponent<ParentComponent>(entity)) {
        auto& parent = scenePtr->GetComponent<ParentComponent>(entity);
        local -= scenePtr->GetComponent<TransformComponent>(parent.GetEntity()).GetWorldTranslation();
    }
    SetLocalTranslation(local);
}

glm::quat TransformComponent::GetWorldRotation() const { return ExtractRotation(GetWorldMatrix()); }
void TransformComponent::SetWorldRotation(glm::quat worldRotation) {
    if (scene.expired()) return;

    std::shared_ptr<Scene> scenePtr = scene.lock();

    glm::quat local = worldRotation;
    if (scenePtr->HasComponent<ParentComponent>(entity)) {
        auto& parent = scenePtr->GetComponent<ParentComponent>(entity);
        local *= glm::inverse(scenePtr->GetComponent<TransformComponent>(parent.GetEntity()).GetWorldRotation());
    }
    SetLocalRotation(local);
}

glm::vec3 TransformComponent::GetLossyScale() const { return ExtractScale(GetWorldMatrix()); }

glm::mat4 TransformComponent::GetWorldMatrix() const {
    if (scene.expired()) return glm::mat4();

    std::shared_ptr<Scene> scenePtr = scene.lock();

    glm::mat4 transform = glm::mat4(1.0f); // identity
    if (scenePtr->HasComponent<ParentComponent>(entity)) {
        auto& parent = scenePtr->GetComponent<ParentComponent>(entity);
        transform = scenePtr->GetComponent<TransformComponent>(parent.GetEntity()).GetLocalMatrix();
    }
    transform = transform * GetLocalMatrix();
    return transform;
}

glm::vec3 TransformComponent::GetLocalTranslation() const { return localTranslation; }
void TransformComponent::SetLocalTranslation(glm::vec3 localTranslation) {
    InsertTranslation(localMatrix, localTranslation);
    this->localTranslation = localTranslation;
}

glm::quat TransformComponent::GetLocalRotation() const { return localRotation; }
void TransformComponent::SetLocalRotation(glm::quat localRotation) {
    InsertRotation(localMatrix, localRotation);
    this->localRotation = localRotation;
}

glm::vec3 TransformComponent::GetLocalScale() const { return localScale; }
void TransformComponent::SetLocalScale(glm::vec3 localScale) {
    InsertScale(localMatrix, localScale);
    this->localScale = localScale;
}

glm::mat4 TransformComponent::GetLocalMatrix() const { return localMatrix; }
void TransformComponent::SetLocalMatrix(glm::mat4 localMatrix) {
    this->localMatrix = localMatrix;
    Decompose(localMatrix, localTranslation, localRotation, localScale);
}
