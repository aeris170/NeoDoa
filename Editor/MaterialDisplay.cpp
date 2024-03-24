#include <Editor/MaterialDisplay.hpp>

#include <imgInspect.h>

#include <Utility/ConstexprConcat.hpp>

#include <Engine/Log.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/ComponentWidgets.hpp>

static void cb(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    glBindSampler(0, *reinterpret_cast<GLuint*>(cmd->UserCallbackData));
}

MaterialDisplay::MaterialDisplay(Observer& observer) noexcept :
    observer(observer) {}

void MaterialDisplay::SetDisplayTarget(Assets& assets, const AssetHandle materialAssetHandle) noexcept {
    this->assets = &assets;
    assert(materialAssetHandle->IsMaterial());
    if (materialAsset != materialAssetHandle) {
        materialAsset = materialAssetHandle;
    }
}

void MaterialDisplay::RenderMessagesTable() noexcept {
    assert(materialAsset.HasValue());
    if (!materialAsset->HasErrorMessages() &&
        !materialAsset->HasWarningMessages() &&
        !materialAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Material Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : materialAsset->ErrorMessages()) {
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
    for (auto& message : materialAsset->WarningMessages()) {
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
    for (auto& message : materialAsset->InfoMessages()) {
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
void MaterialDisplay::RenderProgramCombo() noexcept {
    const auto& programIDs = assets->ShaderProgramAssetIDs();
    Material& material = materialAsset->DataAs<Material>();

    std::string comboPreviewString;
    AssetHandle programHandle = assets->FindAsset(material.ShaderProgram);
    if (programHandle.HasValue()) {
        const ShaderProgram& programInUse = programHandle->DataAs<ShaderProgram>();
        comboPreviewString = programInUse.Name;
    } else {
        comboPreviewString = "<NO SHADER PROGRAM SELECTED>";
    }

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Shader Program: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::BeginCombo("###program_combo", comboPreviewString.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (const auto& programID : programIDs) {
            AssetHandle programAsset = assets->FindAsset(programID);
            assert(programAsset.HasValue());

            const bool isSelected = (material.ShaderProgram == programAsset->ID());
            const ShaderProgram& program = programAsset->DataAs<ShaderProgram>();
            if (ImGui::Selectable(program.Name.c_str(), isSelected)) {
                assets->TryDeleteDependencyBetween(materialAsset->ID(), material.ShaderProgram);
                material.ShaderProgram = programID;
                assets->TryRegisterDependencyBetween(materialAsset->ID(), programID);
                material.ClearAllUniforms();
                materialAsset->Serialize();
                materialAsset->ForceDeserialize();
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
void MaterialDisplay::RenderShaderUniforms() noexcept {
    Material& material = materialAsset->DataAs<Material>();
    if (material.ShaderProgram == UUID::Empty()) { return; }

    static GLuint sampler;
    if (sampler == 0) {
        glGenSamplers(1, &sampler);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }

    ImGui::GetWindowDrawList()->AddCallback(cb, &sampler);

    AssetHandle programHandle = assets->FindAsset(material.ShaderProgram);
    if (!programHandle.HasValue()) {
        ImGui::Text("Selected program (ID: %%llu) is non-existant!", static_cast<uint64_t>(material.ShaderProgram));
        return;
    }

    if (!programHandle->IsShaderProgram()) {
        ImGui::Text("Selected program (ID: %%llu) is non a shader program!", static_cast<uint64_t>(material.ShaderProgram));
        return;
    }

    const ShaderProgram& program = programHandle->DataAs<ShaderProgram>();
    if (!program.IsComplete()) {
        ImGui::Text("Selected program (ID: %%llu) is not complete!", static_cast<uint64_t>(material.ShaderProgram));
        return;
    }

    if (CountUniformsInGroup(program, Shader::ShaderType::Vertex) > 0) {
        ImGui::SeparatorText(std::format("Vertex Shader - {} ({})", assets->FindAsset(program.VertexShader)->DataAs<Shader>().Name, program.VertexShader.AsString()).c_str());
        RenderUniformGroup(material.VertexUniforms, program, Shader::ShaderType::Vertex);
    }

    if (program.TessellationControlShader != UUID::Empty() && CountUniformsInGroup(program, Shader::ShaderType::TessellationControl) > 0) {
        ImGui::SeparatorText(std::format("Tessellation Control Shader - {} ({})", assets->FindAsset(program.TessellationControlShader)->DataAs<Shader>().Name, program.TessellationControlShader.AsString()).c_str());
        RenderUniformGroup(material.VertexUniforms, program, Shader::ShaderType::TessellationControl);
    }

    if (program.TessellationEvaluationShader != UUID::Empty() && CountUniformsInGroup(program, Shader::ShaderType::TessellationEvaluation) > 0) {
        ImGui::SeparatorText(std::format("Tessellation Evaluation Shader - {} ({})", assets->FindAsset(program.TessellationEvaluationShader)->DataAs<Shader>().Name, program.TessellationEvaluationShader.AsString()).c_str());
        RenderUniformGroup(material.VertexUniforms, program, Shader::ShaderType::TessellationEvaluation);
    }

    if (program.GeometryShader != UUID::Empty() && CountUniformsInGroup(program, Shader::ShaderType::Geometry) > 0) {
        ImGui::SeparatorText(std::format("Geometry Shader - {} ({})", assets->FindAsset(program.GeometryShader)->DataAs<Shader>().Name, program.GeometryShader.AsString()).c_str());
        RenderUniformGroup(material.VertexUniforms, program, Shader::ShaderType::Geometry);
    }

    if (CountUniformsInGroup(program, Shader::ShaderType::Fragment) > 0) {
        ImGui::SeparatorText(std::format("Fragment Shader - {} ({})", assets->FindAsset(program.FragmentShader)->DataAs<Shader>().Name, program.FragmentShader.AsString()).c_str());
        RenderUniformGroup(material.VertexUniforms, program, Shader::ShaderType::Fragment);
    }

    textureView.Render();
}

int MaterialDisplay::CountUniformsInGroup(const ShaderProgram& program, Shader::ShaderType group) noexcept {
    return static_cast<int>(std::ranges::count_if(program.Uniforms, [group](const auto& uniform) { return uniform.ReferencedBy == group; }));
}
void MaterialDisplay::RenderUniformGroup(Material::Uniforms& uniforms, const ShaderProgram& program, Shader::ShaderType group) noexcept {
    for (const auto& uniform : program.Uniforms) {
        if (uniform.ReferencedBy != group) { continue; }

        if (!uniforms.TryGet(uniform.Location)) {
            MaterialPostDeserialization::EmplaceUniform(uniforms, uniform.Location, uniform.TypeName, uniform.Name);
        }
        auto& uniformValue = uniforms.Get(uniform.Location);

        if (RenderSingleUniform(uniforms, uniformValue, uniform)) {
            materialAsset->Serialize();
            materialAsset->ForceDeserialize();
        }
    }
}
bool MaterialDisplay::RenderSingleUniform(Material::Uniforms& uniforms, const UniformValue& uniformValue, const ShaderProgram::Uniform& uniform) noexcept {
    bool rv{ false };
    //ImGui::Text("Name: %s, Type: %s", uniform.Name.c_str(), uniform.TypeName.c_str());

    if (uniform.TypeName == "float") {
        Uniform1f value = std::get<Uniform1f>(uniformValue.Value);
        if (FancyVector1Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "vec2") {
        Uniform2f value = std::get<Uniform2f>(uniformValue.Value);
        if (FancyVector2Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "vec3") {
        Uniform3f value = std::get<Uniform3f>(uniformValue.Value);
        if (FancyVector3Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "vec4") {
        Uniform4f value = std::get<Uniform4f>(uniformValue.Value);
        if (FancyVector4Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    }

    //else if (uniform.TypeName == "double") {
    //} else if (uniform.TypeName == "dvec2") {
    //} else if (uniform.TypeName == "dvec3") {
    //} else if (uniform.TypeName == "dvec4") {
    //}

    else if (uniform.TypeName == "int") {
        Uniform1i value = std::get<Uniform1i>(uniformValue.Value);
        if (FancyVectori1Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "ivec2") {
        Uniform2i value = std::get<Uniform2i>(uniformValue.Value);
        if (FancyVectori2Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "ivec3") {
        Uniform3i value = std::get<Uniform3i>(uniformValue.Value);
        if (FancyVectori3Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "ivec4") {
        Uniform4i value = std::get<Uniform4i>(uniformValue.Value);
        if (FancyVectori4Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    }

    else if (uniform.TypeName == "unsigned int") {
        Uniform1ui value = std::get<Uniform1ui>(uniformValue.Value);
        if (FancyVectorui1Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "uvec2") {
        Uniform2ui value = std::get<Uniform2ui>(uniformValue.Value);
        if (FancyVectorui2Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "uvec3") {
        Uniform3ui value = std::get<Uniform3ui>(uniformValue.Value);
        if (FancyVectorui3Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "uvec4") {
        Uniform4ui value = std::get<Uniform4ui>(uniformValue.Value);
        if (FancyVectorui4Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    }

    else if (uniform.TypeName == "bool") {
        Uniform1i value = std::get<Uniform1i>(uniformValue.Value);
        if (FancyVectorb1Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "bvec2") {
        Uniform2i value = std::get<Uniform2i>(uniformValue.Value);
        if (FancyVectorb2Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "bvec3") {
        Uniform3i value = std::get<Uniform3i>(uniformValue.Value);
        if (FancyVectorb3Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    } else if (uniform.TypeName == "bvec4") {
        Uniform4i value = std::get<Uniform4i>(uniformValue.Value);
        if (FancyVectorb4Widget(uniformValue.Name.c_str(), value)) {
            uniforms.Set(uniformValue.Location, uniformValue.Name, value);
            rv = true;
        }
    }

    else if (uniform.TypeName == "sampler1D") {
        ImGui::TextUnformatted("Implementation pending...");
    } else if (uniform.TypeName == "sampler2D") {
        UniformSampler2D value = std::get<UniformSampler2D>(uniformValue.Value);

        const Texture* texture{ nullptr };
        AssetHandle handle = assets->FindAsset(value.textureUUID);
        if (handle && handle->IsTexture()) {
            texture = &handle->DataAs<Texture>();
        } else {
            texture = missingTexture;
        }

        if (Image2DButtonWidget(uniformValue.Name.c_str(), texture->TextureIDRaw())) {
            textureView.Show(*texture);
        }
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::Button(cat(ObserverIcons::MaterialDisplayIcons::ContextMenu::RESET_UNIFORM_ICON, "Reset"))) {
                uniforms.Set(uniform.Location, uniform.Name, UniformSampler2D{ UUID::Empty(), value.samplerUUID });
                rv = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if(ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                UUID data = *(const UUID*) payload->Data;
                AssetHandle handle = assets->FindAsset(data);
                assert(handle.HasValue());
                if (handle->IsTexture()) {
                    uniforms.Set(uniform.Location, uniform.Name, UniformSampler2D{ data, value.samplerUUID });
                    rv = true;
                }
            }
            ImGui::EndDragDropTarget();
        }
    } else if (uniform.TypeName == "sampler3D") {
        ImGui::TextUnformatted("Implementation pending...");
    }
    /*
    else if (uniform.TypeName == "mat2") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix2f>{});
    } else if (uniform.TypeName == "mat3") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix3f>{});
    } else if (uniform.TypeName == "mat4") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix4f>{});
    } else if (uniform.TypeName == "mat2x3") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix2x3f>{});
    } else if (uniform.TypeName == "mat2x4") {
        ///uniforms.Setv(uniform.Location, std::span<UniformMatrix2x4f>{});
    } else if (uniform.TypeName == "mat3x2") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix3x2f>{});
    } else if (uniform.TypeName == "mat3x4") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix3x4f>{});
    } else if (uniform.TypeName == "mat4x2") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix4x2f>{});
    } else if (uniform.TypeName == "mat4x3") {
        //uniforms.Setv(uniform.Location, std::span<UniformMatrix4x3f>{});
    }*/ else {
        ImGui::TextDisabled("Uniform typename %s not currently supported!", uniform.TypeName.c_str());
    }
    return rv;
}

void MaterialDisplay::TextureView::Render() noexcept {
    if (!visible) { return; }
    ImGui::Begin(std::format("Texture View - {}", texture->Name()).c_str(), &visible);
    /* channels */
    static bool r{ true }, g{ true }, b{ true }, a{ true };
    /* inspect params */
    static bool drawInspector{ true }, drawHistogram{ false }, drawNormals{ false };

    float lineHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2;
    float totalBottomPadding = lineHeight * 2.75f;

    auto [windowWidth, windowHeight] = ImGui::GetContentRegionAvail();
    windowWidth = windowWidth - ImGui::GetStyle().FramePadding.x;
    windowHeight = windowHeight - totalBottomPadding;

    float w = static_cast<float>(texture->Width());
    float h = static_cast<float>(texture->Height());
    float aspect = w / h;

    float maxWidth = windowWidth;
    float maxHeight = windowHeight;

    w = maxWidth;
    h = w / aspect;

    if (h > maxHeight) {
        aspect = w / h;
        h = maxHeight;
        w = h * aspect;
    }

    ImGui::Image(texture->TextureIDRaw(), { w, h }, { 0, 1 }, { 1, 0 }, { (float) r, (float) g, (float) b, (float) a }, { 1, 1, 0, 1 });

    if (drawInspector) {
        ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImVec2 mouseUVCoord = (ImGui::GetIO().MousePos - rc.Min) / rc.GetSize();
        mouseUVCoord.y = 1.f - mouseUVCoord.y;
        if (mouseUVCoord.x >= 0.0f &&
            mouseUVCoord.y >= 0.0f &&
            mouseUVCoord.x <= 1.0f &&
            mouseUVCoord.y <= 1.0f) {
            float w = static_cast<float>(texture->Width());
            float h = static_cast<float>(texture->Height());
            auto pixels = reinterpret_cast<const unsigned char*>(Texture::GetByteBufferOf(*texture));
            ImageInspect::inspect(static_cast<int>(w), static_cast<int>(h), pixels, mouseUVCoord, { w, h }, drawNormals, drawHistogram);
        }
    }

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - totalBottomPadding);
    ImGui::AlignTextToFramePadding(); ImGui::Text("Channels");
    ImGui::SameLine(); ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "R:"); ImGui::SameLine(); ImGui::Checkbox("##r", &r);
    ImGui::SameLine(); ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "G:"); ImGui::SameLine(); ImGui::Checkbox("##g", &g);
    ImGui::SameLine(); ImGui::TextColored({ 0.2f, 0.2f, 1.0f, 1.0f }, "B:"); ImGui::SameLine(); ImGui::Checkbox("##b", &b);
    ImGui::SameLine(); ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, "A:"); ImGui::SameLine(); ImGui::Checkbox("##a", &a);

    ImGui::AlignTextToFramePadding(); ImGui::Text("Image Inspect:");   ImGui::SameLine(); ImGui::Checkbox("##inspect", &drawInspector);
    ImGui::SameLine(); ImGui::Text("Normals:");    ImGui::SameLine(); ImGui::Checkbox("##normals", &drawNormals);
    ImGui::SameLine(); ImGui::Text("Histogram:");  ImGui::SameLine(); ImGui::Checkbox("##histogram", &drawHistogram);
    ImGui::End();
}
void MaterialDisplay::TextureView::Show(const Texture& texture) noexcept {
    visible = true;
    this->texture = &texture;
}
void MaterialDisplay::TextureView::Hide() noexcept { visible = false; }