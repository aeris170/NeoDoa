#pragma once

#include <optional>
#include <functional>

#include <imgui.h>
#include <ImGuizmo.h>

#include <Engine/Scene.hpp>

struct GUI;

struct SceneSettings {

    std::reference_wrapper<GUI> gui;

    explicit SceneSettings(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    void DrawStats(Scene& scene) const;
    void DrawSettings(Scene& scene);
};
