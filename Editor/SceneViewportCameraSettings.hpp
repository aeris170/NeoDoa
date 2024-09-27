#pragma once

#include <functional>

#include <imgui.h>

struct GUI;

struct SceneViewportCameraSettings {
    std::reference_wrapper<GUI> gui;

    explicit SceneViewportCameraSettings(GUI& owner) noexcept;

    bool Begin(ImVec2 position) noexcept;
    void Render() noexcept;
    void End() noexcept;

    void Show() noexcept;
    void Hide() noexcept;
private:
    bool isOpen{ false };
    bool isClosing{ false };
};