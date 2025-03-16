#pragma once

#include <functional>

#include <Engine/Color.hpp>
#include <Engine/Assets.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUDescriptorSet.hpp>
#include <Engine/PerspectiveCamera.hpp>

struct Observer;

struct MeshDisplay {

    struct WireframeSettingsData {
        bool renderWireframe{ true };
        std::byte padding[15];
        Color wireframeColor{ 0.0f, 0.0f, 0.0f, 1.0f };
        float wireframeThicknessCoefficient{ 1.5f };
    } WireframeSettings{};
    struct NormalVisualizationSettingsData {
        bool renderNormals{ false };
        std::byte padding[15];
        Color normalColor{ 1.0f, 1.0f, 0.0f, 1.0f };
        float normalMagnitude{ 0.1f };
    } NormalVisualizationSettings{};

    explicit MeshDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle meshAssetHandle) noexcept;
    void RenderMessagesTable() noexcept;
    void RenderMeshPreview(Resolution size) noexcept;
    void RenderPreviewSettings() noexcept;

private:
    std::reference_wrapper<Observer> observer;

    AssetHandle meshAsset{};

    PerspectiveCamera camera{ 80.0f, 16.0f / 9.0f, 0.001f, 1000.0f };
    Resolution availableSize{};
    GPUBuffer projViewBuffer;
    GPUBuffer wireframeBuffer;
    GPUBuffer normalVisualizationBuffer;
    GPUShaderProgram mainProgram;
    GPUPipeline mainPipeline;
    GPUShaderProgram normalVisualizationProgram;
    GPUPipeline normalVisualizationPipeline;
    GPUDescriptorSet mainPerFrame;
    GPUDescriptorSet normalVisualizationPerFrame;
    GPUFrameBuffer framebufferMultisampled;
    GPUFrameBuffer framebuffer;

    void ReallocFrameBufferIfNeeded(Resolution size) noexcept;
    void RenderMeshToOffscreenBuffer() noexcept;

    struct Controls {
        bool rightClicked{ false };
        ImVec2 prevDelta{ 0, 0 };

        float cameraSpeed{ 3 };
        float sensitivity{ 3 };

        float r{ 1 };
        float phi{ 0 };
        float theta{ 90 };
    } controls{};

    void HandleMouseControls() noexcept;

    void ResetCamera() noexcept;

    // context: the thing which we're editing the color of.
    // Like, if we're editing "Wireframe Color", context is "Wirefra"
    void RenderColorButton(std::string_view label, std::string_view colorButtonID, std::string_view popupName, std::string_view popupTitle, Color& color, ImGuiColorEditFlags flags = 0) noexcept;

    void OnAssetRefreshed(AssetHandle handle) noexcept;
};