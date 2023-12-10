#include <Editor/SceneViewport.hpp>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Utility/constexpr_concat.hpp>

#include <Engine/Core.hpp>
#include <Engine/Input.hpp>
#include <Engine/Window.hpp>
#include <Engine/FrameBuffer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>

SceneViewport::SceneViewport(GUI& gui) noexcept :
    gui(gui),
    gizmos(*this) {}

bool SceneViewport::Begin() {
    GUI& gui = this->gui;

    ImGui::PushID(GUI::SCENE_VIEWPORT_TITLE);
    std::string title(WindowIcons::SCENE_VIEWPORT_WINDOW_ICON);
    title.append(GUI::SCENE_VIEWPORT_TITLE);
    title.append(GUI::SCENE_VIEWPORT_ID);
    bool visible = ImGui::Begin(title.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    DrawViewportSettings(gui.HasOpenScene());

    return visible;
}

void SceneViewport::Render() {
    GUI& gui = this->gui;
    if (!gui.HasOpenScene()) { return; }
    Scene& scene = gui.GetOpenScene();

    viewportSize = { static_cast<int>(ImGui::GetContentRegionAvail().x), static_cast<int>(ImGui::GetContentRegionAvail().y) };
    viewportPosition = {
        ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
        ImGui::GetWindowPos().y + ImGui::GetCursorPos().y
    };

    ImVec2 size{ static_cast<float>(viewportSize.Width), static_cast<float>(viewportSize.Height) };
    ImGui::Image(reinterpret_cast<void*>(gui.CORE->FrameBuffer()->GetColorAttachment()), size, { 0, 1 }, { 1, 0 });

    ImGui::PushClipRect({ viewportPosition.x, viewportPosition.y }, { viewportPosition.x + size.x, viewportPosition.y + size.y }, false);
    gizmos.settings.viewportSize = viewportSize;
    gizmos.settings.viewportPosition = viewportPosition;
    gizmos.Render(scene);
    ImGui::PopClipRect();

    DrawCubeControl(scene);
    HandleMouseControls(scene);
}

void SceneViewport::End() {
    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopID();
}

void SceneViewport::DrawViewportSettings(bool hasScene) {
    GUI& gui = this->gui;
    ImFont* font = gui.GetFontBold();
    ImGui::PushFont(font);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

    if (gizmos.settings.enabled) {
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
        if (ImGui::Button(GIZMOS_BUTTON_TEXT)) {
            gizmos.settings.enabled = false;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    } else if (ImGui::Button(GIZMOS_BUTTON_TEXT)) {
        gizmos.settings.enabled = true;
    }
    ImGui::SameLine();

    float lineHeight = font->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
    if (gizmos.settings.mode == ImGuizmo::MODE::WORLD) {
        if (ImGui::Button(cat(SceneViewportIcons::Gizmos::GIZMO_MODE_GLOBAL_ICON, "World"))) {
            gizmos.settings.mode = ImGuizmo::MODE::LOCAL;
        }
    } else if (gizmos.settings.mode == ImGuizmo::MODE::LOCAL) {
        if (ImGui::Button(cat(SceneViewportIcons::Gizmos::GIZMO_MODE_LOCAL_ICON, "Local"))) {
            gizmos.settings.mode = ImGuizmo::MODE::WORLD;
        }
    }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();

    { // Draw Gizmo type buttons group
        if (gizmos.settings.type == ImGuizmo::OPERATION::TRANSLATE) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
            ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_TRANSLATE_ICON, buttonSize);
            ImGui::PopStyleColor(2);
        } else if (ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_TRANSLATE_ICON, buttonSize)) {
            gizmos.settings.type = ImGuizmo::OPERATION::TRANSLATE;
        }
        ImGui::SameLine();

        if (gizmos.settings.type == ImGuizmo::OPERATION::ROTATE) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
            ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_ROTATE_ICON, buttonSize);
            ImGui::PopStyleColor(2);
        } else if (ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_ROTATE_ICON, buttonSize)) {
            gizmos.settings.type = ImGuizmo::OPERATION::ROTATE;
        }
        ImGui::SameLine();

        if (gizmos.settings.type == ImGuizmo::OPERATION::SCALE) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
            ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_SCALE_ICON, buttonSize);
            ImGui::PopStyleColor(2);
        } else if (ImGui::Button(SceneViewportIcons::Gizmos::GIZMO_SCALE_ICON, buttonSize)) {
            gizmos.settings.type = ImGuizmo::OPERATION::SCALE;
        }
        ImGui::SameLine();
    }

    ImGui::Dummy({ ImGui::GetContentRegionAvail().x - buttonSize.x, buttonSize.y });
    ImGui::SameLine();

    if (!hasScene) {
        ImGui::BeginDisabled();
    }
    if (gui.CORE->IsPlaying()) {
        if (ImGui::Button(SceneViewportIcons::STOP_ICON, buttonSize)) {
            gui.CORE->SetPlaying(false);
        }
    } else if (ImGui::Button(SceneViewportIcons::PLAY_ICON, buttonSize)) {
        gui.CORE->SetPlaying(true);
    }
    if (!hasScene) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopFont();
}

void SceneViewport::DrawCubeControl(Scene& scene) {
    auto& camera = scene.GetActiveCamera();
    camera.UpdateView();
    glm::mat4 view = camera._viewMatrix;
    ImGuizmo::SetDrawlist();
    ImGuizmo::ViewManipulate(glm::value_ptr(view), 8, { viewportPosition.x + viewportSize.Width - 128 , viewportPosition.y }, { 128, 128 }, 0x10101080);
    camera.forward = glm::normalize(glm::vec3(-view[0].z, -view[1].z, -view[2].z)); // forward is INVERTED!!!

    // don't change up vector, fuck space sims. up being something other than 0, 1, 0 is VERBOTEN!
    //ptr->_activeCamera->up = glm::normalize(glm::vec3(view[0].y, view[1].y, view[2].y));

    controls.yaw = glm::degrees(atan2(camera.forward.z, camera.forward.x));
    controls.pitch = glm::degrees(asin(camera.forward.y));
}

void SceneViewport::HandleMouseControls(Scene& scene) {
    GUI& gui = this->gui;
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
            gui.window->DisableCursor();
        } else {
            controls.rightClicked = false;
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            // TODO select object, mouse pick
            // glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            // gui.Events.OnEntitySelected(entity);
            // gui.Events.OnEntityDeselected();
        }
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        controls.rightClicked = false;
        controls.prevDelta = { 0, 0 };
        gui.window->EnableCursor();
    }

    if (controls.rightClicked) {
        ImVec2 v = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        ImVec2 delta = { (v.x - controls.prevDelta.x) / 30 * controls.sensitivity, (controls.prevDelta.y - v.y) / 30 * controls.sensitivity };
        if (scene.IsOrtho()) {
            eye.x -= delta.x; // want the movement of the camera to be the inverse of mouse. like
            eye.y -= delta.y; // how your phone's touch screen works. drag right, cam goes left.
        } else if (scene.IsPerspective()) {
            glm::vec3 right = glm::normalize(glm::cross(forward, up)) * (controls.cameraSpeed * gui.delta);
            glm::vec3 fwd = glm::normalize(forward) * (controls.cameraSpeed * gui.delta);
            if (gui.CORE->Input()->IsKeyPressed(KEY_W)) {
                eye += fwd;
            }
            if (gui.CORE->Input()->IsKeyPressed(KEY_A)) {
                eye -= right;
            }
            if (gui.CORE->Input()->IsKeyPressed(KEY_S)) {
                eye -= fwd;
            }
            if (gui.CORE->Input()->IsKeyPressed(KEY_D)) {
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
            assert(false); /* no camera? */
        }
        controls.prevDelta = v;
    }
}
