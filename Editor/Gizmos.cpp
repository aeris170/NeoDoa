#include <Editor/Gizmos.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>

Gizmos::Gizmos(SceneViewport& sv) noexcept :
    sv(sv) {
    sv.gui.get().Events.SceneHierarchy.OnEntitySelected   += std::bind_front(&Gizmos::OnEntitySelected  , this);
    sv.gui.get().Events.SceneHierarchy.OnEntityDeselected += std::bind_front(&Gizmos::OnEntityDeselected, this);
    sv.gui.get().Events.OnEntityDeleted                   += std::bind_front(&Gizmos::OnEntityDeleted   , this);
}

void Gizmos::Render(Scene& scene) {
    if (selectedEntity == NULL_ENTT) { return; }
    if (!settings.enabled) { return; }
    SceneViewport& sv = this->sv.get();

    ImGuizmo::SetDrawlist();
    if (sv.GetViewportCamera().IsPerspective()) {
        ImGuizmo::SetOrthographic(false);
    } else if (sv.GetViewportCamera().IsOrtho()) {
        ImGuizmo::SetOrthographic(true);
    }
    const auto& camera = sv.GetViewportCamera().GetActiveCamera();
    glm::mat4 proj = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    ImGuizmo::SetRect(
        settings.viewportPosition.x,
        settings.viewportPosition.y,
        static_cast<float>(settings.viewportSize.Width),
        static_cast<float>(settings.viewportSize.Height)
    );

    // Entity transform
    glm::mat4 matrix = TransformComponent::ComputeWorldMatrix(selectedEntity, scene);

    bool snap = sv.gui.get().CORE->GetInput()->IsKeyPressed(Key::LeftControl);
    float snapValue = 0.5f;
    if (settings.type == ImGuizmo::OPERATION::ROTATE) { snapValue = 5.0f; }

    ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(proj),
        settings.type,
        settings.mode,
        glm::value_ptr(matrix),
        nullptr,
        snap ? &std::array<float, 3>{snapValue, snapValue, snapValue} [0] : nullptr
    );

    if (ImGuizmo::IsUsing()) {
        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;
        TransformComponent::Decompose(matrix, &translation, &rotation, &scale);

        TransformComponent& transform = scene.GetComponent<TransformComponent>(selectedEntity);

        glm::vec3 currentWorldTranslation = TransformComponent::ComputeWorldTranslation(selectedEntity, scene);
        glm::vec3 translationDifference = translation - currentWorldTranslation;
        glm::vec3 currentLocalTranslation = transform.GetLocalTranslation();
        glm::vec3 newLocalTranslation = currentLocalTranslation + translationDifference;
        transform.SetLocalTranslation(newLocalTranslation);

        glm::quat currentWorldRotation = TransformComponent::ComputeWorldRotation(selectedEntity, scene);
        glm::quat rotationDifference = glm::inverse(currentWorldRotation) * rotation;
        glm::quat currentLocalRotation = transform.GetLocalRotation();
        glm::quat newLocalRotation = currentLocalRotation * rotationDifference;
        transform.SetLocalRotation(newLocalRotation);

        transform.SetLocalScale(scale);
    }
}

void Gizmos::OnEntitySelected(Entity entity) {
    selectedEntity = entity;
}
void Gizmos::OnEntityDeselected() {
    selectedEntity = NULL_ENTT;
}
void Gizmos::OnEntityDeleted(Entity entity) {
    if (selectedEntity == entity) {
        selectedEntity = NULL_ENTT;
    }
}