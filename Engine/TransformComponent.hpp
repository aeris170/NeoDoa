#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "Entity.hpp"

struct Scene;
namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct TransformComponent {
private:
    Entity entity;

    glm::vec3 localTranslation{ glm::vec3(0, 0, 0) };
    glm::quat localRotation{ glm::quat_identity<float, glm::packed_highp>() };
    glm::vec3 localScale{ glm::vec3(1, 1, 1) };
	glm::mat4 localMatrix{ glm::identity<glm::mat4>() };

    std::weak_ptr<Scene> scene;

    TransformComponent(const Entity owner, std::weak_ptr<Scene> scene, glm::mat4&& matrix) noexcept;

public:

    TransformComponent(const Entity owner, std::weak_ptr<Scene> scene) noexcept;

    static void InsertTranslation(glm::mat4& m, const glm::vec3& translation);
    static void InsertRotation(glm::mat4& m, const glm::quat& rotation);
    static void InsertScale(glm::mat4& m, const glm::vec3& scale);
    static void Compose(glm::mat4& m, const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale);

    static glm::vec3 ExtractTranslation(const glm::mat4& m);
    static glm::quat ExtractRotation(const glm::mat4& m);
    static glm::vec3 ExtractScale(const glm::mat4& m);
    static void Decompose(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale);

    Entity GetEntity() const;

    glm::vec3 GetWorldTranslation() const;
    void SetWorldTranslation(glm::vec3 worldTranslation);

    glm::quat GetWorldRotation() const;
    void SetWorldRotation(glm::quat worldRotation);

    glm::vec3 GetLossyScale() const;

    glm::mat4 GetWorldMatrix() const;

    glm::vec3 GetLocalTranslation() const;
    void SetLocalTranslation(glm::vec3 localTranslation);

    glm::quat GetLocalRotation() const;
    void SetLocalRotation(glm::quat localRotation);

    glm::vec3 GetLocalScale() const;
    void SetLocalScale(glm::vec3 localScale);

    glm::mat4 GetLocalMatrix() const;
    void SetLocalMatrix(glm::mat4 localMatrix);

    friend void SerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& transform);
    friend TransformComponent DeserializeTransformComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);

};