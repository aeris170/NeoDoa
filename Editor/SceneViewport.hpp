#pragma once

#include <optional>

#include <imgui.h>
#include <ImGuizmo.h>

struct GUI;
struct Scene;

struct SceneViewport {

	constexpr static auto GIZMOS_BUTTON_TEXT{ "Gizmos" };

	GUI* const gui;

	SceneViewport(GUI* gui) noexcept;

	void Begin(const std::optional<Scene>& scene);
	void Render(Scene& scene);
	void End();

	ImVec2 GetPosition() const;
	ImVec2 GetSize() const;

private:
	struct ImGuizmoSettings {
		bool enabled{ true };
		ImGuizmo::OPERATION type{ ImGuizmo::OPERATION::TRANSLATE };
		ImGuizmo::MODE mode{ ImGuizmo::MODE::LOCAL };
	} gizmoSettings;
	ImVec2 viewportPosition, viewportSize;

	void DrawViewportSettings(const std::optional<Scene>& scene);
	void DrawGizmos(Scene& scene);
	void DrawCubeControl(Scene& scene);

	struct Controls {
		bool rightClicked{ false };
		ImVec2 prevDelta{ 0,0 };

		float cameraSpeed{ 30 };
		float sensitivity{ 3 };

		float yaw{ 0 };
		float pitch{ 0 };
	} controls;

	void HandleMouseControls(Scene& scene);
};
