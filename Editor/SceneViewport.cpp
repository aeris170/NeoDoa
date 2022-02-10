#include "SceneViewport.hpp"

#include <imgui.h>

#include <Core.hpp>
#include <Input.hpp>
#include <Window.hpp>
#include <FrameBuffer.hpp>

#include "GUI.hpp"
#include "Icons.hpp"

SceneViewport::SceneViewport(GUI* gui) noexcept :
	gui(gui) {}

void SceneViewport::Begin(const std::optional<Scene>& scene) {
	ImGui::PushID(gui->SCENE_VIEWPORT_TITLE);
	std::string title(WindowIcons::SCENE_VIEWPORT_WINDOW_ICON);
	title.append(gui->SCENE_VIEWPORT_TITLE);
	title.append(gui->SCENE_VIEWPORT_ID);
	ImGui::Begin(title.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	DrawViewportSettings(scene);

	viewportSize = ImGui::GetContentRegionAvail();
	viewportPosition = {
		ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
		ImGui::GetWindowPos().y + ImGui::GetCursorPos().y
	};
}

void SceneViewport::Render(Scene& scene) {
	ImGui::Image((void*)gui->core->_offscreenBuffer->_tex, { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });

	if (gizmoSettings.enabled) {
		DrawGizmos(scene);
	}
	DrawCubeControl(scene);
	HandleMouseControls(scene);
}

void SceneViewport::End() {
	ImGui::PopStyleVar();
	ImGui::End();
	ImGui::PopID();
}

ImVec2 SceneViewport::GetPosition() const { return viewportPosition; }
ImVec2 SceneViewport::GetSize() const { return viewportSize; }

void SceneViewport::DrawViewportSettings(const std::optional<Scene>& scene) {
	ImFont* font = gui->GetFontBold();
	ImGui::PushFont(font);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

	if (gizmoSettings.enabled) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		if (ImGui::Button(GIZMOS_BUTTON_TEXT)) {
			gizmoSettings.enabled = false;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	} else if (ImGui::Button(GIZMOS_BUTTON_TEXT)) {
		gizmoSettings.enabled = true;
	}
	ImGui::SameLine();

	float lineHeight = font->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
	if (gizmoSettings.mode == ImGuizmo::MODE::WORLD) {
		if (ImGui::Button(ICON_FA_GLOBE " World")) {
			gizmoSettings.mode = ImGuizmo::MODE::LOCAL;
		}
	} else if (gizmoSettings.mode == ImGuizmo::MODE::LOCAL) {
		if (ImGui::Button(ICON_FA_MALE " Local")) {
			gizmoSettings.mode = ImGuizmo::MODE::WORLD;
		}
	}
	ImGui::PopStyleColor(2);
	ImGui::SameLine();

	{ // Draw Gizmo type buttons group
		if (gizmoSettings.type == ImGuizmo::OPERATION::TRANSLATE) {
			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
			ImGui::Button(ICON_FA_ARROWS_ALT, buttonSize);
			ImGui::PopStyleColor(2);
		} else if (ImGui::Button(ICON_FA_ARROWS_ALT, buttonSize)) {
			gizmoSettings.type = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();

		if (gizmoSettings.type == ImGuizmo::OPERATION::ROTATE) {
			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
			ImGui::Button(ICON_FA_SYNC_ALT, buttonSize);
			ImGui::PopStyleColor(2);
		} else if (ImGui::Button(ICON_FA_SYNC_ALT, buttonSize)) {
			gizmoSettings.type = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();

		if (gizmoSettings.type == ImGuizmo::OPERATION::SCALE) {
			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
			ImGui::Button(ICON_FA_EXPAND_ALT, buttonSize);
			ImGui::PopStyleColor(2);
		} else if (ImGui::Button(ICON_FA_EXPAND_ALT, buttonSize)) {
			gizmoSettings.type = ImGuizmo::OPERATION::SCALE;
		}
		ImGui::SameLine();
	}

	ImGui::Dummy({ ImGui::GetContentRegionAvail().x - buttonSize.x, buttonSize.y });
	ImGui::SameLine();

	if (!scene) {
		ImGui::BeginDisabled();
	}
	if (gui->core->_playing) {
		if (ImGui::Button(ICON_FA_STOP, buttonSize)) {
			gui->core->_playing = false;
		}
	} else if (ImGui::Button(ICON_FA_PLAY, buttonSize)) {
		gui->core->_playing = true;
	}
	if (!scene) {
		ImGui::EndDisabled();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopFont();
}

void SceneViewport::DrawGizmos(Scene& scene) {

}

void SceneViewport::DrawCubeControl(Scene& scene) {

}

void SceneViewport::HandleMouseControls(Scene& scene) {
	auto& camera = scene.GetActiveCamera();
	glm::vec3& eye = camera.eye;
	glm::vec3& forward = camera.forward;
	glm::vec3& up = camera.up;
	float& zoom = camera.zoom;

	if (ImGui::IsItemHovered()) {
		zoom += ImGui::GetIO().MouseWheel / 100;
		zoom = std::max(1.f, zoom);
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			controls.rightClicked = true;
			gui->window->DisableCursor();
		} else {
			controls.rightClicked = false;
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			// TODO select object, mouse pick
			// glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}
	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		controls.rightClicked = false;
		controls.prevDelta = { 0, 0 };
		gui->window->EnableCursor();
	}

	if (controls.rightClicked) {
		ImVec2 v = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		ImVec2 delta = { (v.x - controls.prevDelta.x) / 30 * controls.sensitivity, (controls.prevDelta.y - v.y) / 30 * controls.sensitivity };
		if (scene.IsOrtho()) {
			eye.x -= delta.x; // want the movement of the camera to be the inverse of mouse. like
			eye.y -= delta.y; // how your phone's touch screen works. drag right, cam looks left.
		} else if (scene.IsPerspective()) {
			glm::vec3 right = glm::normalize(glm::cross(forward, up)) * (controls.cameraSpeed * gui->delta);
			glm::vec3 fwd = glm::normalize(forward) * (controls.cameraSpeed * gui->delta);
			if (IsKeyPressed(KEY_W)) {
				eye += fwd;
			}
			if (IsKeyPressed(KEY_A)) {
				eye -= right;
			}
			if (IsKeyPressed(KEY_S)) {
				eye -= fwd;
			}
			if (IsKeyPressed(KEY_D)) {
				eye += right;
			}
			controls.yaw += delta.x;
			controls.pitch += delta.y;
			if (controls.pitch > 89.0f) {
				controls.pitch = 89.0f;
			} else if (controls.pitch < -89.0f) {
				controls.pitch = -89.0f;
			}
			glm::vec3 direction;
			direction.x = cos(glm::radians(controls.yaw)) * cos(glm::radians(controls.pitch));
			direction.y = sin(glm::radians(controls.pitch));
			direction.z = sin(glm::radians(controls.yaw)) * cos(glm::radians(controls.pitch));
			forward = glm::normalize(direction);
		} else {
			assert(false, "no camera?");
		}
		controls.prevDelta = v;
	}
}