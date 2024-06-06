#include <Editor/SceneViewport.hpp>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Utility/ConstexprConcat.hpp>

#include <Engine/Core.hpp>
#include <Engine/Input.hpp>
#include <Engine/Window.hpp>
#include <Engine/Graphics.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUDescriptorSet.hpp>

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

GPUPipeline pipe;
GPUShaderProgram prog;
GPUDescriptorSet perFrame;
GPUDescriptorSet perObject;
const GPUTexture* tex;
GPUSampler sampler;
GPUBuffer buf;
GPUBuffer perFrameUniformBuffer;
GPUBuffer perObjectUniformBuffer;
SceneViewport::SceneViewport(GUI& gui) noexcept :
    gui(gui),
    gizmos(*this) {

    float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    };
    const std::byte* bytePtr = reinterpret_cast<const std::byte*>(vertices);

    GPUBufferBuilder bBuilder;
    buf = bBuilder.SetStorage(std::span<const std::byte>(bytePtr, 180 * sizeof(float))).Build().first.value();
    perFrameUniformBuffer = bBuilder.SetProperties(BufferProperties::DynamicStorage).SetStorage(sizeof(glm::mat4) * 2, nullptr).Build().first.value(); // proj and view
    perObjectUniformBuffer = bBuilder.SetProperties(BufferProperties::DynamicStorage).SetStorage(sizeof(glm::mat4), nullptr).Build().first.value(); // model
    GPUVertexAttribLayout layout;
    layout.Define<float>(3);
    layout.Define<float>(2);

    GPUShaderBuilder sBuilder;
    auto v = sBuilder.SetType(ShaderType::Vertex).SetSourceCode(R"(
#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

out vec2 fUV;

layout(std140, binding = 0) uniform ProjViewBuffer {
    mat4 projection;
    mat4 view;
};

layout(std140, binding = 1) uniform ModelBuffer {
    mat4 model;
};

void main() {
	gl_Position = projection * view * model * vec4(vPos, 1.0);
    fUV = vUV;
}
    )").Build().first;
    auto f = sBuilder.SetType(ShaderType::Fragment).SetSourceCode(R"(
#version 460 core

in vec2 fUV;

layout (binding = 0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
	FragColor = texture(tex, fUV) * vec4(1,1,1,1);
}
)").Build().first;

    GPUShaderProgramBuilder spBuilder;
    prog = spBuilder.SetVertexShader(v.value()).SetFragmentShader(f.value()).Build().first.value();

    GPUPipelineBuilder aBuilder;
    pipe = aBuilder
        .SetArrayBuffer(0, buf, layout)
        .SetTopology(TopologyType::Triangles)
        .SetPolygonMode(PolygonMode::Fill)
        .SetViewport({ 0, 0, viewportSize.Width, viewportSize.Height })
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetMultisampleEnabled(true)
        .SetShaderProgram(prog)
        .Build().first.value();

    tex = &Core::GetCore()->GetAssetGPUBridge()->GetTextures().Missing();

    GPUSamplerBuilder saBuilder;
    sampler = saBuilder
        .SetMagnificationFilter(TextureMagnificationMode::Nearest)
        .Build().first.value();

    GPUDescriptorSetBuilder dsBuilder;
    perFrame = dsBuilder
        .SetUniformBufferBinding(0, perFrameUniformBuffer)
        .Build().first.value();

    perObject = dsBuilder
        .SetUniformBufferBinding(1, perObjectUniformBuffer)
        .SetCombinedImageSamplerBinding(0, *tex, sampler)
        .Build().first.value();
}


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
    ReallocBufferIfNeeded({ static_cast<unsigned>(ImGui::GetContentRegionAvail().x), static_cast<unsigned>(ImGui::GetContentRegionAvail().y) });
    RenderSceneToBuffer(scene);

    ImVec2 size{ static_cast<float>(viewportSize.Width), static_cast<float>(viewportSize.Height) };
    ImGui::Image(reinterpret_cast<void*>(static_cast<uint64_t>(std::get<GPUTexture>(viewportFramebuffer.ColorAttachments[0].value()).GLObjectID)), size, { 0, 1 }, { 1, 0 });
    //ImGui::Image(reinterpret_cast<void*>(static_cast<uint64_t>(Core::GetCore()->GetFrameBuffer()->GetColorAttachment())), size, { 0, 1 }, { 1, 0 });

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
    viewportCamera.GetPerspectiveCamera()._aspect = size.Aspect();

    GPUTextureBuilder tBuilder;
    auto&& color = tBuilder
        .SetWidth(viewportSize.Width)
        .SetHeight(viewportSize.Height)
        .SetData(DataFormat::RGBA16F, {})
        //.SetSamples(Multisample::x8)
        .Build().first;
    assert(color.has_value());

    GPURenderBufferBuilder rbBuilder;
    auto&& depthStencil = rbBuilder.SetLayout(viewportSize.Width, viewportSize.Height, DataFormat::DEPTH32F_STENCIL8).Build().first;
    assert(depthStencil.has_value());

    GPUFrameBufferBuilder fbBuilder;
    fbBuilder.SetName("NeoDoa Editor Scene Viewport Buffer")
        .AttachColorTexture(std::move(color.value()), 0)
        .AttachDepthStencilRenderBuffer(std::move(depthStencil.value()));
    auto&& fb = fbBuilder.Build().first;
    assert(fb.has_value());

    viewportFramebuffer = std::move(fb.value());
    pipe.Viewport = { 0, 0, viewportSize.Width, viewportSize.Height };
}
void SceneViewport::RenderSceneToBuffer(Scene& scene) {
    //scene.Update(gui.get().delta);
    //scene.Render();
    if (gizmos.selectedEntity == NULL_ENTT) return;


    Graphics::SetRenderTarget(viewportFramebuffer);
    Graphics::BindPipeline(pipe);
    Graphics::ClearRenderTarget(viewportFramebuffer, { scene.ClearColor.r, scene.ClearColor.g, scene.ClearColor.b, scene.ClearColor.a });

    // Bind per-frame uniform
    viewportCamera.GetPerspectiveCamera().UpdateView();
    viewportCamera.GetPerspectiveCamera().UpdateProjection();
    glm::mat4 matrices[2] {
        viewportCamera.GetPerspectiveCamera()._projectionMatrix,
        viewportCamera.GetPerspectiveCamera()._viewMatrix
    };
    GPUBuffer::BufferSubData(perFrameUniformBuffer, sizeof(matrices), reinterpret_cast<NonOwningPointerToConstRawData>(glm::value_ptr(matrices[0])));
    Graphics::BindDescriptorSet(perFrame);
    // ---

    // Bind per-object uniform
    const auto& t = scene.GetComponent<TransformComponent>(gizmos.selectedEntity);
    glm::mat4 model = t.GetLocalMatrix();
    GPUBuffer::BufferSubData(perObjectUniformBuffer, sizeof(model), reinterpret_cast<NonOwningPointerToConstRawData>(glm::value_ptr(model)));
    Graphics::BindDescriptorSet(perObject);
    // ---

    Graphics::Render(36);
    Graphics::SetRenderTarget({});




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