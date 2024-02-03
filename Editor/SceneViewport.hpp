#pragma once

#include <optional>
#include <functional>

#include <glm/glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Engine/Scene.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/FrameBuffer.hpp>

#include <Editor/Gizmos.hpp>

struct GUI;

struct SceneViewport {

    constexpr static auto GIZMOS_BUTTON_TEXT{ "Gizmos" };

    struct ViewportCamera {
        void SwitchToOrtho();
        void SwitchToPerspective();

        ACamera& GetActiveCamera();
        OrthoCamera& GetOrthoCamera();
        PerspectiveCamera& GetPerspectiveCamera();

        bool IsOrtho() const;
        bool IsPerspective() const;

    private:
        ACamera* activeCamera{ &ortho };
        OrthoCamera ortho{ -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f };
        PerspectiveCamera perspective{ 110.0f, 19.0f / 9.0f, 0.001f, 1000.0f };
    };

    std::reference_wrapper<GUI> gui;
    Gizmos gizmos;

    explicit SceneViewport(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

    ViewportCamera& GetViewportCamera();

private:
    glm::vec2 viewportPosition{};
    Resolution viewportSize{};
    std::unique_ptr<FrameBuffer> viewportFramebuffer{ nullptr };

    void ReallocBufferIfNeeded(Resolution size);
    void RenderSceneToBuffer(Scene& scene);

    void DrawViewportSettings(bool hasScene);
    void DrawCubeControl();

    ViewportCamera viewportCamera{};

    struct Controls {
        bool rightClicked{ false };
        ImVec2 prevDelta{ 0, 0 };

        float cameraSpeed{ 30 };
        float sensitivity{ 3 };

        float yaw{ 0 };
        float pitch{ 0 };
    } controls{};

    void HandleMouseControls();
};
