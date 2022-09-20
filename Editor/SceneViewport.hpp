#pragma once

#include <optional>
#include <functional>

#include <glm\glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Resolution.hpp>

#include "Gizmos.hpp"

struct GUI;
struct Scene;

struct SceneViewport {

	constexpr static auto GIZMOS_BUTTON_TEXT{ "Gizmos" };

	std::reference_wrapper<GUI> gui;
	Gizmos gizmos;

	SceneViewport(GUI& gui) noexcept;

	void Begin(Scene* scene);
	void Render(Scene& scene);
	void End();

private:
	glm::vec2 viewportPosition{};
	Resolution viewportSize{};

	void DrawViewportSettings(Scene* scene);
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
