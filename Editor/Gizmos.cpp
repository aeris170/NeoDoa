#include <Editor/Gizmos.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>

Gizmos::Gizmos(SceneViewport& sv) noexcept :
    sv(sv) {}

void Gizmos::Render(Scene& scene) {
    GUI& gui = sv.get().gui;
    if (gui.sh.selectedEntity == NULL_ENTT) { return; }
    if (!settings.enabled) { return; }

    ImGuizmo::SetDrawlist();
    if (scene.IsPerspective()) {
        ImGuizmo::SetOrthographic(false);
        scene.GetPerspective()._aspect = settings.viewportSize.Aspect();
    } else if (scene.IsOrtho()) {
        ImGuizmo::SetOrthographic(true);
    }
    const auto& camera = scene.GetActiveCamera();
    glm::mat4 proj = camera._projectionMatrix;
    glm::mat4 view = camera._viewMatrix;

    ImGuizmo::SetRect(
        settings.viewportPosition.x,
        settings.viewportPosition.y,
        static_cast<float>(settings.viewportSize.Width),
        static_cast<float>(settings.viewportSize.Height)
    );

    // Entity transform
    TransformComponent& transformComponent = scene.GetComponent<TransformComponent>(gui.sh.selectedEntity);
    glm::mat4 matrix = transformComponent.GetWorldMatrix();

    bool snap = gui.CORE->Input()->IsKeyPressed(KEY_LEFT_CONTROL);
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