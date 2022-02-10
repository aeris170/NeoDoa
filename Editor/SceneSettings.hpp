#pragma once

#include <optional>

#include <imgui.h>
#include <ImGuizmo.h>

struct GUI;
struct Scene;

struct SceneSettings {

	GUI* const gui;

	SceneSettings(GUI* gui) noexcept;

	void Begin(const std::optional<Scene>& scene);
	void Render(Scene& scene);
	void End();

};
