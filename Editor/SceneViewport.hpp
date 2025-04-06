#pragma once

#include <optional>
#include <functional>

#include <glm/glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Engine/UUID.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Resolution.hpp>

#include <Engine/Graphics.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUDescriptorSet.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>

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
        ACamera* activeCamera{ &perspective };
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

    ImVec2 GetViewportCameraSettingsButtonPosition() const noexcept;

private:
    glm::vec2 viewportPosition{};
    Resolution viewportSize{};
    GPUFrameBuffer viewportFramebufferMultisampled;
    GPUFrameBuffer viewportFramebuffer;

    ImVec2 viewportCameraSettingsButtonPosition;

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

    struct ObjectState {
        GPUPipeline pipeline;
        GPUBuffer uniformModelBuffer;
        GPUBuffer uniformInstanceTransformsBuffer;
        GPUDescriptorSet descriptorSet;
        const GPUBuffer* commandBuffer;
        unsigned count;
    };
    struct Renderer {
        GPUVertexAttribLayout layout;
        GPUShaderProgram program;

        GPUBuffer perFrameUniformBuffer;
        GPUDescriptorSet perFrame;

        const GPUTexture* missingTexture;
        GPUSampler defaultSampler;
        GPUBuffer buf;

        std::vector<Entity> objectIDs{};
        std::vector<ObjectState> objectStates{};
    } renderer{};

    void HandleMouseControls();
};
