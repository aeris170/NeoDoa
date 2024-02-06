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
        sv.GetViewportCamera().GetPerspectiveCamera()._aspect = settings.viewportSize.Aspect();
    } else if (sv.GetViewportCamera().IsOrtho()) {
        ImGuizmo::SetOrthographic(true);
    }
    const auto& camera = sv.GetViewportCamera().GetActiveCamera();
    glm::mat4 proj = camera._projectionMatrix;
    glm::mat4 view = camera._viewMatrix;

    ImGuizmo::SetRect(
        settings.viewportPosition.x,
        settings.viewportPosition.y,
        static_cast<float>(settings.viewportSize.Width),
        static_cast<float>(settings.viewportSize.Height)
    );

    // Entity transform
    TransformComponent& transformComponent = scene.GetComponent<TransformComponent>(selectedEntity);
    glm::mat4 matrix = transformComponent.GetWorldMatrix();

    bool snap = sv.gui.get().CORE->GetInput()->IsKeyPressed(KEY_LEFT_CONTROL);
    float snapValue = 0.5f;
    if (settings.type == ImGuizmo::OPERATION::ROTATE) { snapValue = 5.0f; }

    ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(proj),
        settings.type,
        settings.mode,
        glm::value_ptr(matrix),
        nullptr,
        snap ? &std::array<float, 3>{snapValue, snapValue, snapValue} [0] : nullptr);

    if (ImGuizmo::IsUsing()) {
        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;
        TransformComponent::Decompose(matrix, translation, rotation, scale);
        transformComponent.SetWorldTranslation(translation);
        transformComponent.SetWorldRotation(rotation);
        transformComponent.SetLocalScale(scale);
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