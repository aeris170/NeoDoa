#pragma once

#include <optional>
#include <functional>

#include <imgui.h>
#include <ImGuizmo.h>

struct GUI;
struct Scene;

struct SceneSettings {

	std::reference_wrapper<GUI> gui;

	SceneSettings(GUI& gui) noexcept;

	void Begin(Scene* scene);
	void Render(Scene& scene);
	void End();

private:

	void DrawStats(Scene& scene);
	void DrawSettings(Scene& scene);
};
