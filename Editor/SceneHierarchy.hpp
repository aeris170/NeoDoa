#pragma once

#include <functional>

#include <imgui.h>

#include <Engine/Scene.hpp>
#include <Engine/Event.hpp>
#include <Engine/Entity.hpp>
#include <Engine/Assets.hpp>

struct GUI;

struct SceneHierarchy {

    ImGuiTreeNodeFlags panel_flags = ImGuiTreeNodeFlags_SpanAvailWidth; // ImGui flag
    const float highlight_expire = 2; // time for highlight to fully disappear
    float _highlightTime = 0; // time since the highligh began
    Entity lastHighlighted = { NULL_ENTT };

    std::reference_wrapper<GUI> gui;

    explicit SceneHierarchy(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    Entity highlightedEntity{ NULL_ENTT };
    Entity deletedEntity{ NULL_ENTT };

    void RenderEntityNode(const Entity entity);
    void RenderContextMenu(const Entity entity);

    Entity selectedEntity{ NULL_ENTT };
    void OnEntitySelected(Entity entity);
    void OnEntityDeselected();
    void OnEntityDeleted(Entity entity);
};