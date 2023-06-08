#pragma once

#include <functional>

#include <imgui.h>

#include <Engine/Entity.hpp>

struct GUI;
struct Scene;

struct SceneHierarchy {

    ImGuiTreeNodeFlags panel_flags = ImGuiTreeNodeFlags_SpanAvailWidth; // ImGui flag
    const float highlight_expire = 2; // time for highlight to fully disappear
    float _highlightTime = 0; // time since the highligh began
    Entity lastHighlighted = { NULL_ENTT };

    std::reference_wrapper<GUI> gui;

    Entity selectedEntity{ NULL_ENTT };
    Entity highlightedEntity{ NULL_ENTT };
    Entity deletedEntity{ NULL_ENTT };

    explicit SceneHierarchy(GUI& gui) noexcept;

    bool Begin();
    void Render(Scene& scene);
    void End();

private:
    void RenderEntityNode(Scene& scene, const Entity entity);
    void RenderContextMenu(Scene& scene, const Entity entity);

    void SetSelectedEntity(Entity entt);
    void ResetSelectedEntity();
};