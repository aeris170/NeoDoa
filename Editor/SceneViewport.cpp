#include <Editor/SceneViewport.hpp>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Utility/ConstexprConcat.hpp>

#include <Engine/Core.hpp>
#include <Engine/Input.hpp>
#include <Engine/Window.hpp>
#include <Engine/FrameBuffer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>

void SceneViewport::ViewportCamera::SwitchToOrtho() { activeCamera = &ortho; }
void SceneViewport::ViewportCamera::SwitchToPerspective() { activeCamera = &perspective; }

ACamera& SceneViewport::ViewportCamera::GetActiveCamera() { return *activeCamera; }
OrthoCamera& SceneViewport::ViewportCamera::GetOrthoCamera() { return ortho; }
PerspectiveCamera& SceneViewport::ViewportCamera::GetPerspectiveCamera() { return perspective; }

bool SceneViewport::ViewportCamera::IsOrtho() const { return activeCamera == &ortho; }
bool SceneViewport::ViewportCamera::IsPerspective() const { return activeCamera == &perspective; }

SceneViewport::SceneViewport(GUI& gui) noexcept :
    gui(gui),
    gizmos(*this) {}


bool SceneViewport::Begin() {
    GUI& gui = this->gui;

    ImGui::PushID(WindowStrings::SceneViewportWindowName);
    bool visible = ImGui::Begin(WindowStrings::SceneViewportWindowTitleID);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    DrawViewportSettings(gui.HasOpenScene());

    return visible;
}

void SceneViewport::Render() {
    GUI& gui = this->gui;
    if (!gui.HasOpenScene()) { return; }
    Scene& scene = gui.GetOpenScene();

    viewportPosition = {
        ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
        ImGui::GetWindowPos().y + ImGui::GetCursorPos().y
    };
    ReallocBufferIfNeeded({ static_cast<int>(ImGui::GetContentRegionAvail().x), static_cast<int>(ImGui::GetContentRegionAvail().y) });
    RenderSceneToBuffer(scene);

    ImVec2 size{ static_cast<float>(viewportSize.Width), static_cast<float>(viewportSize.Height) };
    ImGui::Image(reinterpret_cast<void*>(viewportFramebuffer->GetColorAttachment()), size, { 0, 1 }, { 1, 0 });

    ImGui::PushClipRect({ viewportPosition.x, viewportPosition.y }, { viewportPosition.x + size.x, viewportPosition.y + size.y }, false);
    gizmos.settings.viewportSize = viewportSize;
    gizmos.settings.viewportPosition = viewportPosition;
    gizmos.Render(scene);
    ImGui::PopClipRect();

    DrawCubeControl();
    HandleMouseControls();
}

void SceneViewport::End() {
    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopID();
}

SceneViewport::ViewportCamera& SceneViewport::GetViewportCamera() { return viewportCamera; }

void SceneViewport::ReallocBufferIfNeeded(Resolution size) {
    if (viewportSize == size) { return; }
    viewportSize = size;
    viewportFramebuffer = std::move(FrameBufferBuilder().SetResolution(size).AddColorAttachment(OpenGL::RGB8).BuildUnique());
}
void SceneViewport::RenderSceneToBuffer(Scene& scene) {
    viewportFramebuffer->Bind();
    scene.Update(gui.get().delta);
    scene.Render();
    FrameBuffer::BackBuffer().Bind();
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

    ImGui::Dummy({ buttonSize.x * 2, buttonSize.y });
    ImGui::SameLine();

    ImGui::BeginDisabled(!gui.HasOpenScene());
    if (ImGui::Button(WindowStrings::SceneViewportCameraSettingsWindowTitle)) {
        gui.GetSceneViewportCameraSettings().Show();
    }
    ImGui::SameLine();

    if (viewportCamera.IsOrtho()) {
        if (ImGui::Button("2D")) {
            viewportCamera.SwitchToPerspective();
        }
    } else {
        if (ImGui::Button("3D")) {
            viewportCamera.SwitchToOrtho();
        }
    }
    ImGui::EndDisabled();
    ImGui::SameLine();

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

void SceneViewport::DrawCubeControl() {
    auto& camera = viewportCamera.GetActiveCamera();
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

void SceneViewport::HandleMouseControls() {
    GUI& gui = this->gui;
    auto& camera = viewportCamera.GetActiveCamera();
    glm::vec3& eye = camera.eye;
    glm::vec3& forward = camera.forward;
    glm::vec3& up = camera.up;
    float& zoom = camera.zoom;

    if (ImGui::IsItemHovered()) {
        zoom += ImGui::GetIO().MouseWheel / 100;
        zoom = std::max(1.f, zoom);
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            controls.rightClicked = true;
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            //gui.window->SetMouseCursor(Window::Cursors::Hand);
            //gui.window->HideCursor();
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
        //gui.window->EnableCursor();
        //gui.window->SetMouseCursor(Window::Cursors::Normal);
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    if (controls.rightClicked) {
        ImVec2 v = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        ImVec2 delta = { (v.x - controls.prevDelta.x) / 30 * controls.sensitivity, (controls.prevDelta.y - v.y) / 30 * controls.sensitivity };
        if (viewportCamera.IsOrtho()) {
            eye.x -= delta.x; // want the movement of the camera to be the inverse of mouse. like
            eye.y -= delta.y; // how your phone's touch screen works. drag right, cam goes left.
        } else if (viewportCamera.IsPerspective()) {
            glm::vec3 right = glm::normalize(glm::cross(forward, up)) * (controls.cameraSpeed * gui.delta);
            glm::vec3 fwd = glm::normalize(forward) * (controls.cameraSpeed * gui.delta);
            if (gui.CORE->GetInput()->IsKeyPressed(KEY_W)) {
                eye += fwd;
            }
            if (gui.CORE->GetInput()->IsKeyPressed(KEY_A)) {
                eye -= right;
            }
            if (gui.CORE->GetInput()->IsKeyPressed(KEY_S)) {
                eye -= fwd;
            }
            if (gui.CORE->GetInput()->IsKeyPressed(KEY_D)) {
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