#pragma once

#include <optional>
#include <functional>

#include <glm/glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Engine/Scene.hpp>
#include <Engine/Resolution.hpp>

#include <Editor/Gizmos.hpp>

struct GUI;

struct SceneViewport {

    constexpr static auto GIZMOS_BUTTON_TEXT{ "Gizmos" };

    std::reference_wrapper<GUI> gui;
    Gizmos gizmos;

    explicit SceneViewport(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    glm::vec2 viewportPosition{};
    Resolution viewportSize{};

    void DrawViewportSettings(bool hasScene);
    void DrawCubeControl(Scene& scene);

    struct Controls {
        bool rightClicked{ false };
        ImVec2 prevDelta{ 0,0 };

        float cameraSpeed{ 30 };
        float sensitivity{ 3 };

        float yaw{ 0 };
        float pitch{ 0 };
    } controls{};

    void HandleMouseControls(Scene& scene);
};
