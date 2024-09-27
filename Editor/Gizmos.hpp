#pragma once

#include <functional>

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/glm.hpp>

#include <Engine/Entity.hpp>
#include <Engine/Resolution.hpp>

struct Scene;
struct SceneViewport;

struct Gizmos {

    struct GizmoSettings {
        bool enabled{ true };
        ImGuizmo::OPERATION type{ ImGuizmo::OPERATION::TRANSLATE };
        ImGuizmo::MODE mode{ ImGuizmo::MODE::LOCAL };
        glm::vec2 viewportPosition{};
        Resolution viewportSize{};
    } settings;

    explicit Gizmos(SceneViewport& sv) noexcept;

    void Render(Scene& scene);

    Entity selectedEntity{ NULL_ENTT };
private:
    std::reference_wrapper<SceneViewport> sv;

    // -- //
    void OnEntitySelected(Entity entity);
    void OnEntityDeselected();
    void OnEntityDeleted(Entity entity);
};

