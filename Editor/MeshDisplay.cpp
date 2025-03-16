#include <Editor/MeshDisplay.hpp>

#include <numbers>
#include <algorithm>

#include <Engine/Core.hpp>
#include <Engine/Mesh.hpp>
#include <Engine/AssetBridge.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/ComponentWidgets.hpp>
#include <glm/gtx/string_cast.hpp>

MeshDisplay::MeshDisplay(Observer& observer) noexcept :
    observer(observer) {
    observer.gui.get().Events.Observer.OnAssetRefreshed += std::bind_front(&MeshDisplay::OnAssetRefreshed, this);
    camera.Eye = { 0, 0, 1 };

    GPUBufferBuilder bBuilder;
    projViewBuffer = bBuilder.SetProperties(BufferProperties::DynamicStorage).SetStorage(sizeof(glm::mat4) * 3, nullptr).Build().first.value();
    wireframeBuffer = bBuilder.SetProperties(BufferProperties::DynamicStorage).SetStorage(sizeof(WireframeSettingsData), nullptr).Build().first.value();
    normalVisualizationBuffer = bBuilder.SetProperties(BufferProperties::DynamicStorage).SetStorage(sizeof(NormalVisualizationSettingsData), nullptr).Build().first.value();

    GPUShaderBuilder sBuilder;
    auto v = sBuilder.SetType(ShaderType::Vertex).SetSourceCode(R"(
#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec2 vUV;

out VS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;
} vs_out;

layout(std140, binding = 0) uniform ProjViewModelBuffer {
    mat4 projection;
    mat4 view;
    mat4 model;
};

void main() {
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    vs_out.normal = vNormal;
    vs_out.color = vColor;
    vs_out.UV = vUV;
}
    )").Build().first;
    auto g = sBuilder.SetType(ShaderType::Geometry).SetSourceCode(R"(
#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;
} gs_in[];

out GS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;

    vec3 wireframeDist;
} gs_out;

void main() {
    for(int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;

        gs_out.normal = gs_in[i].normal;
        gs_out.color = gs_in[i].color;
        gs_out.UV = gs_in[i].UV;

        // This is the easiest scheme I could think of. The attribute will be interpolated, so
        // all you have to do is set the ith dimension to 1.0 to get barycentric coordinates
        // specific to this triangle. The frag shader will interpolate and then you can just use
        // a threshold in the frag shader to figure out if you're close to an edge
        gs_out.wireframeDist = vec3(0.0);
        gs_out.wireframeDist[i] = 1.0;

        EmitVertex();
    }
}
)").Build().first;
    auto f = sBuilder.SetType(ShaderType::Fragment).SetSourceCode(R"(
#version 460 core

in GS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;

    vec3 wireframeDist;
} fs_in;

layout(std140, binding = 1) uniform WireframeBuffer {
    bool renderWireframe;
    vec4 wireframeColor;
    float wireframeThicknessCoefficient;
};

out vec4 FragColor;

void main() {
    vec3 thickness = fwidth(fs_in.wireframeDist) * wireframeThicknessCoefficient;

    vec3 a3 = smoothstep(vec3(0), thickness, fs_in.wireframeDist);
    float edgeFactor = mix(min(min(a3.x, a3.y), a3.z), 1, !renderWireframe);

    vec4 frontFacingColor = mix(wireframeColor, fs_in.color, edgeFactor);
    vec4 backFacingColor  = vec4(wireframeColor.rgb, wireframeColor.a * (1 - edgeFactor));
    FragColor = mix(backFacingColor, frontFacingColor, gl_FrontFacing);
}
)").Build().first;
    GPUShaderProgramBuilder spBuilder;
    mainProgram = spBuilder.SetVertexShader(v.value()).SetGeometryShader(g.value()).SetFragmentShader(f.value()).Build().first.value();

    v = sBuilder.SetType(ShaderType::Vertex).SetSourceCode(R"(
#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec2 vUV;

out VS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;
} vs_out;

layout(std140, binding = 0) uniform ProjViewModelBuffer {
    mat4 projection;
    mat4 view;
    mat4 model;
};

void main() {
    gl_Position = view * model * vec4(vPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * vNormal, 0.0)));
    vs_out.color = vColor;
    vs_out.UV = vUV;
}
    )").Build().first;
    g = sBuilder.SetType(ShaderType::Geometry).SetSourceCode(R"(
#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec4 color;
    vec2 UV;
} gs_in[];

layout(std140, binding = 0) uniform ProjViewModelBuffer {
    mat4 projection;
    mat4 view;
    mat4 model;
};
layout(std140, binding = 1) uniform NormalBuffer {
    bool renderNormals;
    vec4 normalColor;
    float normalMagnitude;
};

void main() {
    for(int i = 0; i < 3; i++) {
        gl_Position = projection * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = projection * (gl_in[i].gl_Position + vec4(gs_in[i].normal, 0.0) * normalMagnitude);
        EmitVertex();
        EndPrimitive();
    }
}
)").Build().first;
    f = sBuilder.SetType(ShaderType::Fragment).SetSourceCode(R"(
#version 460 core

layout(std140, binding = 1) uniform NormalBuffer {
    bool renderNormals;
    vec4 normalColor;
    float normalMagnitude;
};

out vec4 FragColor;

void main() {
    FragColor = normalColor;
}
)").Build().first;
    normalVisualizationProgram = spBuilder.SetVertexShader(v.value()).SetGeometryShader(g.value()).SetFragmentShader(f.value()).Build().first.value();

    GPUDescriptorSetBuilder dsBuilder;
    mainPerFrame = dsBuilder
        .SetUniformBufferBinding(0, projViewBuffer)
        .SetUniformBufferBinding(1, wireframeBuffer)
        .Build().first.value();

    normalVisualizationPerFrame = dsBuilder
        .SetUniformBufferBinding(0, projViewBuffer)
        .SetUniformBufferBinding(1, normalVisualizationBuffer)
        .Build().first.value();
}

void MeshDisplay::SetDisplayTarget(const AssetHandle meshAssetHandle) noexcept {
    static auto& Core{ Core::GetCore() };

    assert(meshAssetHandle->IsMesh());
    if (meshAsset != meshAssetHandle) {
        meshAsset = meshAssetHandle;
        const Mesh& m = meshAsset->DataAs<Mesh>();

        const auto& bridge{ Core->GetAssetGPUBridge() };
        assert(bridge->GetVertexBuffers().Query(meshAsset->ID()));
        GPUVertexAttribLayout layout;
        layout.Define<float>(3); // Position
        layout.Define<float>(3); // Normal
        layout.Define<float>(4); // Color
        layout.Define<float>(2); // UV

        GPUPipelineBuilder aBuilder;
        aBuilder.SetName("NeoDoa MeshDisplay Main Pipeline")
            .SetArrayBuffer(0, bridge->GetVertexBuffers().Fetch(meshAsset->ID()), layout)
            .SetViewport({ 0, 0, availableSize.Width, availableSize.Height })
            .SetDepthTestEnabled(true)
            .SetDepthWriteEnabled(true)
            .SetDepthClampEnabled(true)
            .SetMultisampleEnabled(true)
            .SetBlendEnabled(true)
            .SetBlendFunction(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha)
            .SetShaderProgram(mainProgram);

        if (!m.Indices.empty()) {
            assert(bridge->GetIndexBuffers().Query(meshAsset->ID()));
            aBuilder.SetIndexBuffer(bridge->GetIndexBuffers().Fetch(meshAsset->ID()), DataType::UnsignedInt);
        }
        mainPipeline = aBuilder.Build().first.value();

        aBuilder.SetName("NeoDoa MeshDisplay Normal Visualization Pipeline")
            .SetShaderProgram(normalVisualizationProgram);
        normalVisualizationPipeline = aBuilder.Build().first.value();

        ResetCamera();
    }
}
void MeshDisplay::RenderMessagesTable() noexcept {
    assert(meshAsset.HasValue());
    if (!meshAsset->HasErrorMessages() &&
        !meshAsset->HasWarningMessages() &&
        !meshAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Mesh Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : meshAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : meshAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : meshAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void MeshDisplay::RenderMeshPreview(Resolution size) noexcept {
    ReallocFrameBufferIfNeeded(size);

    RenderMeshToOffscreenBuffer();

    ImGui::Image(
        std::get<GPUTexture>(framebuffer.ColorAttachments[0].value()),
        { static_cast<float>(size.Width), static_cast<float>(size.Height) },
        { 0, 1 }, { 1, 0 },
        { 1, 1, 1, 1 },
        { 1, 1, 0, 1 }
    );

    HandleMouseControls();
}
void MeshDisplay::RenderPreviewSettings() noexcept {
    if (ImGui::BeginTable("##model_display_mesh_preview_settings_table", 2)) {
        ImGui::TableNextColumn();
        ImGui::Checkbox("Wireframe:", &WireframeSettings.renderWireframe);
        ImGui::TableNextColumn();
        ImGui::Checkbox("Normals:", &NormalVisualizationSettings.renderNormals);

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!WireframeSettings.renderWireframe);
        ImGui::Indent();
        ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Thickness:");
        ImGui::SameLine(); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x); ImGui::SliderFloat("##wireframe_thickness_coeff", &WireframeSettings.wireframeThicknessCoefficient, std::numeric_limits<float>::epsilon(), 3, "%.7f", ImGuiSliderFlags_ClampOnInput);
        ImGui::Unindent();
        ImGui::EndDisabled();

        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!NormalVisualizationSettings.renderNormals);
        ImGui::Indent();
        ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Magnitude:");
        ImGui::SameLine(); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x); ImGui::SliderFloat("##normals_magnitude", &NormalVisualizationSettings.normalMagnitude, std::numeric_limits<float>::epsilon(), 3, "%.7f", ImGuiSliderFlags_ClampOnInput);
        ImGui::Unindent();
        ImGui::EndDisabled();

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!WireframeSettings.renderWireframe);
        ImGui::Indent(); {
            ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_HDR |
                ImGuiColorEditFlags_NoAlpha |
                ImGuiColorEditFlags_NoLabel;
            RenderColorButton("Color:", "##wireframe_color", "wireframe_color_picker", "Wireframe Color", WireframeSettings.wireframeColor, picker_flags);
        } ImGui::Unindent();
        ImGui::EndDisabled();

        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!NormalVisualizationSettings.renderNormals);
        ImGui::Indent(); {
            ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_HDR |
                ImGuiColorEditFlags_AlphaBar |
                ImGuiColorEditFlags_NoLabel |
                ImGuiColorEditFlags_AlphaPreviewHalf;
            RenderColorButton("Color:", "##normals_color", "normals_color_picker", "Normals Color", NormalVisualizationSettings.normalColor, picker_flags);
        } ImGui::Unindent();
        ImGui::EndDisabled();

        ImGui::EndTable();
    }
}

void MeshDisplay::ReallocFrameBufferIfNeeded(Resolution size) noexcept {
    if (availableSize == size) { return; }
    availableSize = size;
    camera.AspectRatio = size.Aspect();

    { // Build Multisampled FB
        GPURenderBufferBuilder rbBuilder;
        auto&& color = rbBuilder
            .SetLayout(availableSize.Width, availableSize.Height, DataFormat::RGBA16F)
            .SetSamples(Multisample::x8)
            .Build().first;
        assert(color.has_value());

        auto&& depthStencil = rbBuilder
            .SetLayout(availableSize.Width, availableSize.Height, DataFormat::DEPTH32F_STENCIL8)
            .SetSamples(Multisample::x8)
            .Build().first;
        assert(depthStencil.has_value());

        GPUFrameBufferBuilder fbBuilder;
        fbBuilder.SetName("NeoDoa Editor MeshDisplay FrameBuffer (MS)")
            .AttachColorRenderBuffer(std::move(color.value()), 0)
            .AttachDepthStencilRenderBuffer(std::move(depthStencil.value()));
        auto&& fb = fbBuilder.Build().first;
        assert(fb.has_value());

        framebufferMultisampled = std::move(fb.value());
    }

    { // Build FB
        GPUTextureBuilder tBuilder;
        auto&& color = tBuilder
            .SetWidth(availableSize.Width)
            .SetHeight(availableSize.Height)
            .SetData(DataFormat::RGBA16F, {})
            .SetSamples(Multisample::None)
            .Build().first;
        assert(color.has_value());

        GPURenderBufferBuilder rbBuilder;
        auto&& depthStencil = rbBuilder.SetLayout(availableSize.Width, availableSize.Height, DataFormat::DEPTH32F_STENCIL8).Build().first;
        assert(depthStencil.has_value());

        GPUFrameBufferBuilder fbBuilder;
        fbBuilder.SetName("NeoDoa Editor MeshDisplay FrameBuffer")
            .AttachColorTexture(std::move(color.value()), 0)
            .AttachDepthStencilRenderBuffer(std::move(depthStencil.value()));
        auto&& fb = fbBuilder.Build().first;
        assert(fb.has_value());

        framebuffer = std::move(fb.value());
    }

    mainPipeline.Viewport = { 0, 0, availableSize.Width, availableSize.Height };
    normalVisualizationPipeline.Viewport = { 0, 0, availableSize.Width, availableSize.Height };
}
void MeshDisplay::RenderMeshToOffscreenBuffer() noexcept {
    std::array<unsigned, 1> targets{ 0 };
    Graphics::SetRenderTarget(framebufferMultisampled, targets);
    Graphics::ClearRenderTarget(framebufferMultisampled, { 0.3f, 0.3f, 0.3f, 1.0f });

    // Allocate and fill per-frame uniform
    camera.UpdateView();
    camera.UpdateProjection();
    std::array<glm::mat4, 3> matrices {
        camera.GetProjectionMatrix(),
        camera.GetViewMatrix(),
        glm::identity<glm::mat4>()
    };
    // Calculate model matrix - fit object into default zoom level and center it.
    const Mesh& m = meshAsset->DataAs<Mesh>();
    glm::vec3 halfExtents = (m.Max - m.Min) * 0.5f;
    float scaleFactor = 1.0f / glm::compMax(halfExtents); // Use the largest half extent
    glm::mat4& model{ matrices[2] };
    model = glm::scale(model, { scaleFactor, scaleFactor, scaleFactor });
    model = glm::translate(model, -m.Origin);

    // Bind per-frame uniform
    Graphics::BufferSubData(projViewBuffer, sizeof(matrices), reinterpret_cast<NonOwningPointerToConstRawData>(glm::value_ptr(matrices[0])));
    Graphics::BufferSubData(wireframeBuffer, sizeof(WireframeSettingsData), reinterpret_cast<NonOwningPointerToConstRawData>(&WireframeSettings));
    Graphics::BufferSubData(normalVisualizationBuffer, sizeof(NormalVisualizationSettingsData), reinterpret_cast<NonOwningPointerToConstRawData>(&NormalVisualizationSettings));

    int count = !m.Indices.empty() ? m.Indices.size() : m.Vertices.size();

    Graphics::BindPipeline(mainPipeline);
    Graphics::BindDescriptorSet(mainPerFrame);
    Graphics::Render(count);

    if (NormalVisualizationSettings.renderNormals) {
        Graphics::BindPipeline(normalVisualizationPipeline);
        Graphics::BindDescriptorSet(normalVisualizationPerFrame);
        Graphics::Render(count);
    }

    Graphics::SetRenderTarget({});

    std::array<unsigned, 1> dst{ 0 };
    Graphics::BlitColor(framebufferMultisampled, framebuffer, 0, dst);
}

void MeshDisplay::HandleMouseControls() noexcept {
    glm::vec3& eye = camera.Eye;
    glm::vec3& forward = camera.Forward;

    if (ImGui::IsItemHovered()) {
        controls.r -= ImGui::GetIO().MouseWheel / 25;
        controls.r = std::max(0.1f, controls.r);
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            controls.rightClicked = true;
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        } else {
            controls.rightClicked = false;
        }
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        controls.rightClicked = false;
        controls.prevDelta = { 0, 0 };
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    if (controls.rightClicked) {
        ImVec2 v = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        ImVec2 delta = { (controls.prevDelta.x - v.x) / 30 * controls.sensitivity, (controls.prevDelta.y - v.y) / 30 * controls.sensitivity };

        controls.phi   += delta.x;
        controls.theta += delta.y;
        controls.theta = std::clamp(
            controls.theta,
            std::numeric_limits<float>::round_error(),
            180.0f - std::numeric_limits<float>::round_error()
        );

        controls.prevDelta = v;
    }

    // https://en.wikipedia.org/wiki/Spherical_coordinate_system
    eye = {
        controls.r * std::sin(glm::radians(controls.theta)) * std::sin(glm::radians(controls.phi)),
        controls.r * std::cos(glm::radians(controls.theta)),
        controls.r * std::sin(glm::radians(controls.theta)) * std::cos(glm::radians(controls.phi))
    };
    forward = glm::normalize(-eye); // Make camera look at origin by setting forward to norm(origin - eye);
}

void MeshDisplay::ResetCamera() noexcept {
    controls.r = 1;
    controls.phi = 0;
    controls.theta = 90;

    glm::vec3& eye = camera.Eye;
    glm::vec3& forward = camera.Forward;

    eye = { 0, 0, 1 };
    forward = { 0, 0, -1 };
}

void MeshDisplay::RenderColorButton(std::string_view label, std::string_view colorButtonID, std::string_view popupName, std::string_view popupTitle, Color& color, ImGuiColorEditFlags pickerFlags) noexcept {
    ImVec4 c = color.ToImVec4();

    ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted(label.data());
    ImGui::SameLine();
    auto& ctx = *ImGui::GetCurrentContext();
    if (ImGui::ColorButton(colorButtonID.data(), c, pickerFlags)) {
        ctx.ColorPickerRef = c;
        ImGui::OpenPopup(popupName.data());
        ImVec2 v;
        v.x = ctx.LastItemData.Rect.GetBL().x - 1;
        v.y = ctx.LastItemData.Rect.GetBL().y + ImGui::GetStyle().ItemSpacing.y;
        ImGui::SetNextWindowPos(v);
    }

    if (ImGui::BeginPopup(popupName.data())) {
        ImGui::TextEx(popupTitle.data());
        ImGui::Spacing();

        ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f);
        if (ImGui::NeoDoaColorPickerPopup("##picker", &c.x, pickerFlags, &ctx.ColorPickerRef.x)) {
            color = Color(c);
        }
        ImGui::EndPopup();
    }
}
void MeshDisplay::OnAssetRefreshed(AssetHandle handle) noexcept {
    // Asset purged its data and allocated new data. Pipelines need corrections for Vertex/Index buffer references. Old ones are gone...
    if (meshAsset == handle) {
        assert(handle->IsMesh());

        meshAsset = {}; // SetDisplayTarget returns if meshAsset == handle. Force it to do work.
        SetDisplayTarget(handle);
    }
}