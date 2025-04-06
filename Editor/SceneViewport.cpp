#include <Editor/SceneViewport.hpp>

#include <utility>
#include <execution>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Utility/ConstexprConcat.hpp>

#include <Engine/Core.hpp>
#include <Engine/Input.hpp>
#include <Engine/Model.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Window.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/RigidModelComponent.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>
/*
void AddModelToScene(Scene& scene, const Model& model) {
    static constexpr auto recurse = [](Scene& scene, Entity parentEntity, const Model& model, size_t nodeIndex, auto&& selfReference) -> void {
        assert(model.Nodes.HasNodeAt(nodeIndex));
        const Model::Node& node = model.Nodes.NodeAt(nodeIndex);

        Entity thisEntity = scene.CreateEntity(node.Name);

        assert(scene.HasComponent<ParentComponent>(parentEntity));
        scene.GetComponent<ParentComponent>(parentEntity).GetChildren().push_back(thisEntity);

        scene.EmplaceComponent<ChildComponent>(thisEntity, parentEntity);

        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;
        TransformComponent::Decompose(node.LocalTransformation, &translation, &rotation, &scale);

        TransformComponent& transform = scene.GetComponent<TransformComponent>(thisEntity);
        transform.SetLocalTranslation(translation);
        transform.SetLocalRotation(rotation);
        transform.SetLocalScale(scale);

        const auto childIndices = model.Nodes.ChildrenOfNodeAt(nodeIndex).ChildIndices();
        if (childIndices.size() > 0 || node.MeshIndices.size() > 0) {
            std::vector<Entity> childrens{};
            childrens.reserve(childIndices.size() + node.MeshIndices.size());
            scene.EmplaceComponent<ParentComponent>(thisEntity, std::move(childrens));
        }
        for (auto childIndex : childIndices) {
            selfReference(scene, thisEntity, model, childIndex, selfReference);
        }
        for (auto meshIndex : node.MeshIndices) {
            assert(meshIndex >= 0 && meshIndex < model.Meshes.size());
            Entity meshEntity = scene.CreateEntity(model.MeshNames[meshIndex]);
            scene.GetComponent<ParentComponent>(thisEntity).GetChildren().push_back(meshEntity);
            scene.EmplaceComponent<ChildComponent>(meshEntity, thisEntity);
        }
    };
    Entity rootEntity = scene.CreateEntity(model.Name);
    const auto childIndices = model.Nodes.ChildrenOfNodeAt(model.Nodes.Root).ChildIndices();
    if (childIndices.size() > 0) {
        std::vector<Entity> childrens{};
        childrens.reserve(childIndices.size());
        scene.EmplaceComponent<ParentComponent>(rootEntity, std::move(childrens));
    }
    for (auto childIndex : childIndices) {
        recurse(scene, rootEntity, model, childIndex, recurse);
    }
}
*/

void SceneViewport::ViewportCamera::SwitchToOrtho() { activeCamera = &ortho; }
void SceneViewport::ViewportCamera::SwitchToPerspective() { activeCamera = &perspective; }

ACamera& SceneViewport::ViewportCamera::GetActiveCamera() { return *activeCamera; }
OrthoCamera& SceneViewport::ViewportCamera::GetOrthoCamera() { return ortho; }
PerspectiveCamera& SceneViewport::ViewportCamera::GetPerspectiveCamera() { return perspective; }

bool SceneViewport::ViewportCamera::IsOrtho() const { return activeCamera == &ortho; }
bool SceneViewport::ViewportCamera::IsPerspective() const { return activeCamera == &perspective; }

SceneViewport::SceneViewport(GUI& gui) noexcept :
    gui(gui),
    gizmos(*this) {

    renderer.layout.Define<float>(3);
    renderer.layout.Define<float>(3);
    renderer.layout.Define<float>(2);

    GPUShaderBuilder sBuilder;
    auto v = sBuilder.SetType(ShaderType::Vertex).SetSourceCode(R"(
#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out vec3 fNormal;
out vec2 fUV;

layout(std140, binding = 0) uniform ProjViewBuffer {
    mat4 projection;
    mat4 view;
};

layout(std140, binding = 1) uniform ModelBuffer {
    mat4 model;
};

layout(std140, binding = 2) restrict readonly buffer InstanceTransformBuffer {
    mat4 instanceTransforms[];
};

void main() {
    int globalInstance = gl_BaseInstance + gl_InstanceID;
    gl_Position = projection * view * model * instanceTransforms[globalInstance] * vec4(vPos, 1.0);
    fNormal = transpose(inverse(mat3(instanceTransforms[globalInstance]))) * vNormal;
    fUV = vUV;
}
    )").Build().first;
    auto f = sBuilder.SetType(ShaderType::Fragment).SetSourceCode(R"(
#version 460 core

in vec3 fNormal;
in vec2 fUV;

layout(binding = 0) uniform sampler2D albedo;
layout(binding = 1) uniform sampler2D roughnessMap;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D metallicMap;

out vec4 FragColor;

void main() {
    //FragColor = texture(albedo, fUV) * vec4(1, 1, 1, 1);
    FragColor = vec4(fNormal, 1);
}
)").Build().first;

    GPUShaderProgramBuilder spBuilder;
    renderer.program = spBuilder
        .SetVertexShader(v.value())
        .SetFragmentShader(f.value()).Build()
        .first.value();

    GPUBufferBuilder bBuilder;
    renderer.perFrameUniformBuffer = bBuilder
        .SetProperties(BufferProperties::DynamicStorage)
        .SetStorage(sizeof(glm::mat4) * 2, nullptr)
        .Build().first.value(); // proj and view

    GPUDescriptorSetBuilder dsBuilder;
    renderer.perFrame = dsBuilder
        .SetUniformBufferBinding(0, renderer.perFrameUniformBuffer)
        .Build().first.value();

    renderer.missingTexture = &Core::GetCore()->GetAssetGPUBridge()->GetTextures().Missing();

    GPUSamplerBuilder saBuilder;
    renderer.defaultSampler = saBuilder
        .SetMinificationFilter(TextureMinificationMode::LinearMipmapLinear)
        .SetMagnificationFilter(TextureMagnificationMode::Linear)
        .SetWrapS(TextureWrappingMode::ClampToEdge)
        .SetWrapT(TextureWrappingMode::ClampToEdge)
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
    ImGui::Image(std::get<GPUTexture>(viewportFramebuffer.ColorAttachments[0].value()), size, { 0, 1 }, { 1, 0 });
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            Assets& assets = *Core::GetCore()->GetAssets();

            UUID droppedAssetID = *reinterpret_cast<UUID*>(payload->Data);
            if (assets.IsModelAsset(droppedAssetID)) {
                const Model& model = assets.GetDataOfAssetAs<Model>(droppedAssetID);
                Entity entt = scene.CreateEntity(model.Name);
                scene.EmplaceComponent<RigidModelComponent>(entt, droppedAssetID);

                renderer.objectIDs.emplace_back(entt);
                ObjectState& state = renderer.objectStates.emplace_back();

                GPUPipelineBuilder pBuilder;
                state.pipeline = pBuilder
                    .SetFaceCullEnabled(true)
                    .SetCullMode(CullMode::Back)
                    .SetArrayBuffer(0, assets.GPUBridge().GetVertexBuffers().Fetch(droppedAssetID), renderer.layout)
                    .SetIndexBuffer(assets.GPUBridge().GetIndexBuffers().Fetch(droppedAssetID), DataType::UnsignedInt)
                    .SetTopology(TopologyType::Triangles)
                    .SetPolygonMode(PolygonMode::Fill)
                    .SetViewport({ 0, 0, viewportSize.Width, viewportSize.Height })
                    .SetDepthTestEnabled(true)
                    .SetDepthWriteEnabled(true)
                    .SetMultisampleEnabled(true)
                    .SetShaderProgram(renderer.program)
                    .Build().first.value();

                GPUBufferBuilder bBuilder;
                state.uniformModelBuffer = bBuilder
                    .SetProperties(BufferProperties::DynamicStorage)
                    .SetStorage(
                        sizeof(glm::mat4),
                        nullptr
                    )
                    .Build().first.value();

                state.uniformInstanceTransformsBuffer = bBuilder
                    .SetProperties(BufferProperties::DynamicStorage)
                    .SetStorage(
                        sizeof(glm::mat4) * model.MeshWorldTransforms.size(),
                        reinterpret_cast<const std::byte*>(model.MeshWorldTransforms.data())
                    )
                    .Build().first.value();

                GPUDescriptorSetBuilder dBuilder;
                // re-create descriptor sets because renderer.objectStates.emplace_back(); moves around elements.
                for (auto& state : renderer.objectStates) {
                    state.descriptorSet = dBuilder
                        .SetUniformBufferBinding(1, state.uniformModelBuffer)
                        .SetStorageBufferBinding(2, state.uniformInstanceTransformsBuffer)
                        .Build().first.value();
                }
                // TODO bind textures etc. to this set too

                // TODO use assets
                state.commandBuffer = assets.GPUBridge().GetCommandBuffers().Query(droppedAssetID);

                state.count = model.MeshWorldTransforms.size();
            }
        }
        ImGui::EndDragDropTarget();
    }

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

ImVec2 SceneViewport::GetViewportCameraSettingsButtonPosition() const noexcept { return viewportCameraSettingsButtonPosition; }

void SceneViewport::ReallocBufferIfNeeded(Resolution size) {
    if (viewportSize == size) { return; }
    viewportSize = size;
    viewportCamera.GetPerspectiveCamera().AspectRatio = size.Aspect();

    { // Build Multisampled FB
        GPURenderBufferBuilder rbBuilder;
        auto&& color = rbBuilder
            .SetLayout(viewportSize.Width, viewportSize.Height, DataFormat::RGBA16F)
            .SetSamples(Multisample::x8)
            .Build().first;
        assert(color.has_value());

        auto&& depthStencil = rbBuilder
            .SetLayout(viewportSize.Width, viewportSize.Height, DataFormat::DEPTH32F_STENCIL8)
            .SetSamples(Multisample::x8)
            .Build().first;
        assert(depthStencil.has_value());

        GPUFrameBufferBuilder fbBuilder;
        fbBuilder.SetName("NeoDoa Editor Scene Viewport Buffer (MS)")
            .AttachColorRenderBuffer(std::move(color.value()), 0)
            .AttachDepthStencilRenderBuffer(std::move(depthStencil.value()));
        auto&& fb = fbBuilder.Build().first;
        assert(fb.has_value());

        viewportFramebufferMultisampled = std::move(fb.value());
    }

    { // Build FB
        GPUTextureBuilder tBuilder;
        auto&& color = tBuilder
            .SetWidth(viewportSize.Width)
            .SetHeight(viewportSize.Height)
            .SetData(DataFormat::RGBA16F, {})
            .SetSamples(Multisample::None)
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
    }

    for (auto& state : renderer.objectStates) {
        state.pipeline.Viewport = { 0, 0, viewportSize.Width, viewportSize.Height };
    }
}
void SceneViewport::RenderSceneToBuffer(Scene& scene) {
    std::array<unsigned, 1> targets{ 0 };
    Graphics::SetRenderTarget(viewportFramebufferMultisampled, targets);
    Graphics::ClearRenderTarget(viewportFramebufferMultisampled, { scene.ClearColor.r, scene.ClearColor.g, scene.ClearColor.b, scene.ClearColor.a });

    // Bind per-frame uniforms
    viewportCamera.GetPerspectiveCamera().UpdateView();
    viewportCamera.GetPerspectiveCamera().UpdateProjection();
    glm::mat4 matrices[2] {
        viewportCamera.GetPerspectiveCamera().GetProjectionMatrix(),
        viewportCamera.GetPerspectiveCamera().GetViewMatrix()
    };
    Graphics::BufferSubData(renderer.perFrameUniformBuffer, sizeof(matrices), reinterpret_cast<NonOwningPointerToConstRawData>(glm::value_ptr(matrices[0])));
    Graphics::BindDescriptorSet(renderer.perFrame);

    // Render rigid model objects
    for (auto [entity, transform, rigidModel] : scene.GetRegistry().view<TransformComponent, RigidModelComponent>().each()) {
        auto search = std::ranges::find(renderer.objectIDs, entity);
        assert(search != renderer.objectIDs.end()); // TODO this will crash in the future if someone else adds into the scene, handle appropriately with events idgaf

        auto index = search - renderer.objectIDs.begin();
        assert(index >= 0 && index < renderer.objectStates.size());

        // Update model matrix data...
        ObjectState& state = renderer.objectStates[index];
        auto data = transform.ComputeWorldMatrix(entity, scene);
        std::span<const std::byte> dataView{
            reinterpret_cast<const std::byte*>(&data),
            sizeof(data)
        };
        Graphics::BufferSubData(state.uniformModelBuffer, dataView);

        // Bind required structs and render object
        Graphics::BindPipeline(state.pipeline);
        Graphics::BindDescriptorSet(state.descriptorSet);
        Graphics::BindCommandBuffer(*state.commandBuffer); // Required for multi indirect rendering...
        Graphics::RenderMultiIndirect(state.count);
    }

    Graphics::SetRenderTarget({});

    std::array<unsigned, 1> dst{ 0 };
    Graphics::BlitColor(viewportFramebufferMultisampled, viewportFramebuffer, 0, dst);
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
    viewportCameraSettingsButtonPosition = ImGui::GetWindowPos() + ImGui::GetCursorPos();
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
    glm::mat4 view = camera.GetViewMatrix();
    ImGuizmo::SetDrawlist();
    ImGuizmo::ViewManipulate(glm::value_ptr(view), 8, { viewportPosition.x + viewportSize.Width - 128 , viewportPosition.y }, { 128, 128 }, 0x10101080);
    camera.Forward = glm::normalize(glm::vec3(-view[0].z, -view[1].z, -view[2].z)); // forward is INVERTED!!!

    // don't change up vector, fuck space sims. up being something other than 0, 1, 0 is VERBOTEN!
    //ptr->_activeCamera->up = glm::normalize(glm::vec3(view[0].y, view[1].y, view[2].y));

    controls.yaw = glm::degrees(atan2(camera.Forward.z, camera.Forward.x));
    controls.pitch = glm::degrees(asin(camera.Forward.y));
}

void SceneViewport::HandleMouseControls() {
    GUI& gui = this->gui;
    auto& camera = viewportCamera.GetActiveCamera();
    glm::vec3& eye = camera.Eye;
    glm::vec3& forward = camera.Forward;
    glm::vec3& up = camera.Up;
    float& zoom = camera.Zoom;

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
            if (gui.CORE->GetInput()->IsKeyDepressed(Key::W)) {
                eye += fwd;
            }
            if (gui.CORE->GetInput()->IsKeyDepressed(Key::A)) {
                eye -= right;
            }
            if (gui.CORE->GetInput()->IsKeyDepressed(Key::S)) {
                eye -= fwd;
            }
            if (gui.CORE->GetInput()->IsKeyDepressed(Key::D)) {
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
            std::unreachable();
        }
        controls.prevDelta = v;
    }
}