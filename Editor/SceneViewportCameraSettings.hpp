#pragma once

#include <functional>

struct GUI;

struct SceneViewportCameraSettings {
    std::reference_wrapper<GUI> gui;

    explicit SceneViewportCameraSettings(GUI& owner) noexcept;

    bool Begin() noexcept;
    void Render() noexcept;
    void End() noexcept;

    void Show() noexcept;
    void Hide() noexcept;
private:
    bool isOpen{ false };
};