#include <Editor/ShaderProgramDisplay.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>

ShaderProgramDisplay::ShaderProgramDisplay(Observer& observer) noexcept :
    observer(observer) {
    vertexShaderText = std::string(128, 0);
    tessCtrlShaderText = std::string(128, 0);
    tessEvalShaderText = std::string(128, 0);
    geometryShaderText = std::string(128, 0);
    fragmentShaderText = std::string(128, 0);
}
void ShaderProgramDisplay::SetDisplayTarget(Assets& assets, const AssetHandle shaderProgramAssetHandle) noexcept {
    this->assets = &assets;
    assert(shaderProgramAssetHandle->IsShaderProgram());
    if (shaderProgramAsset != shaderProgramAssetHandle) {
        shaderProgramAsset = shaderProgramAssetHandle;
        const ShaderProgram& program = shaderProgramAsset->DataAs<ShaderProgram>();

        if (program.HasVertexShader()) {
            UUID uuid = program.VertexShader;
            vertexShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            vertexShaderText = NOT_SET_MANDATORY_TEXT;
        }

        if (program.HasTessellationControlShader()) {
            UUID uuid = program.TessellationControlShader;
            tessCtrlShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            tessCtrlShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasTessellationEvaluationShader()) {
            UUID uuid = program.TessellationEvaluationShader;
            tessEvalShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            tessEvalShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasGeometryShader()) {
            UUID uuid = program.GeometryShader;
            geometryShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            geometryShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasFragmentShader()) {
            UUID uuid = program.FragmentShader;
            fragmentShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            fragmentShaderText = NOT_SET_MANDATORY_TEXT;
        }
    }
}

void ShaderProgramDisplay::RenderMessagesTable() noexcept {
    if (!shaderProgramAsset->HasErrorMessages() &&
        !shaderProgramAsset->HasWarningMessages() &&
        !shaderProgramAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("linker_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Linker Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : shaderProgramAsset->ErrorMessages()) {
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
    for (auto& message : shaderProgramAsset->WarningMessages()) {
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
    for (auto& message : shaderProgramAsset->InfoMessages()) {
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
void ShaderProgramDisplay::RenderShaders() noexcept {
    ShaderProgram& program = shaderProgramAsset->DataAs<ShaderProgram>();
    RenderVertexShader(program);
    RenderTessCtrlShader(program);
    RenderTessEvalShader(program);
    RenderGeometryShader(program);
    RenderFragmentShader(program);
}

void ShaderProgramDisplay::Begin(std::string_view label) noexcept {
    float w = ImGui::GetContentRegionAvail().x;

    ImGui::PushID(label.data());
    ImGui::Columns(2, nullptr, false);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
    ImGui::SetColumnWidth(0, 240);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f + 3);
    ImGui::TextDisabled("%s", label.data());
    ImGui::NextColumn();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);

    float buttonWidth;
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[1];
        buttonWidth = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    }

    ImGui::PushItemWidth(w - 240 - buttonWidth);
    ImGuiIO& io = ImGui::GetIO();
    auto font = io.Fonts->Fonts[0];
    ImGui::PushFont(font);
}
void ShaderProgramDisplay::RenderVertexShader(ShaderProgram& program) noexcept {
    ImGui::BeginDisabled();
    if (program.HasVertexShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_MANDATORY_COLOR);
    }
    Begin("Vertex Shader");
    ImGui::InputText("###vertex_shader_text", vertexShaderText.data(), vertexShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::ShaderType::Vertex) {
                    assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.VertexShader);
                    program.VertexShader = data;
                    assets->TryRegisterDependencyBetween(shaderProgramAsset->ID(), data);
                    shaderProgramAsset->Serialize();
                    shaderProgramAsset->ForceDeserialize();
                    vertexShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    ImGui::PushFont(boldFont);
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .94f, .51f, .55f, 1 });
    if (ImGui::Button("X", buttonSize)) {
        assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.VertexShader);
        program.VertexShader = 0;
        shaderProgramAsset->Serialize();
        shaderProgramAsset->ForceDeserialize();
        vertexShaderText = NOT_SET_MANDATORY_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void ShaderProgramDisplay::RenderTessCtrlShader(ShaderProgram& program) noexcept {
    ImGui::BeginDisabled();
    if (program.HasTessellationControlShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Tessellation Control Shader");
    ImGui::InputText("###tess_ctrl_shader_text", tessCtrlShaderText.data(), tessCtrlShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::ShaderType::TessellationControl) {
                    assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.TessellationControlShader);
                    program.TessellationControlShader = data;
                    assets->TryRegisterDependencyBetween(shaderProgramAsset->ID(), data);
                    shaderProgramAsset->Serialize();
                    shaderProgramAsset->ForceDeserialize();
                    tessCtrlShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    ImGui::PushFont(boldFont);
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .94f, .51f, .55f, 1 });
    if (ImGui::Button("X", buttonSize)) {
        assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.TessellationControlShader);
        program.TessellationControlShader = 0;
        shaderProgramAsset->Serialize();
        shaderProgramAsset->ForceDeserialize();
        tessCtrlShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void ShaderProgramDisplay::RenderTessEvalShader(ShaderProgram& program) noexcept {
    ImGui::BeginDisabled();
    if (program.HasTessellationEvaluationShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Tessellation Evaluation Shader");
    ImGui::InputText("###tess_eval_shader_text", tessEvalShaderText.data(), tessEvalShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::ShaderType::TessellationEvaluation) {
                    assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.TessellationEvaluationShader);
                    program.TessellationEvaluationShader = data;
                    assets->TryRegisterDependencyBetween(shaderProgramAsset->ID(), data);
                    shaderProgramAsset->Serialize();
                    shaderProgramAsset->ForceDeserialize();
                    tessEvalShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    ImGui::PushFont(boldFont);
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .94f, .51f, .55f, 1 });
    if (ImGui::Button("X", buttonSize)) {
        assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.TessellationEvaluationShader);
        program.TessellationEvaluationShader = 0;
        shaderProgramAsset->Serialize();
        shaderProgramAsset->ForceDeserialize();
        tessEvalShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void ShaderProgramDisplay::RenderGeometryShader(ShaderProgram& program) noexcept {
    ImGui::BeginDisabled();
    if (program.HasGeometryShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Geometry Shader");
    ImGui::InputText("###geometry_text", geometryShaderText.data(), geometryShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::ShaderType::Geometry) {
                    assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.GeometryShader);
                    program.GeometryShader = data;
                    assets->TryRegisterDependencyBetween(shaderProgramAsset->ID(), data);
                    shaderProgramAsset->Serialize();
                    shaderProgramAsset->ForceDeserialize();
                    geometryShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    ImGui::PushFont(boldFont);
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .94f, .51f, .55f, 1 });
    if (ImGui::Button("X", buttonSize)) {
        assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.GeometryShader);
        program.GeometryShader = 0;
        shaderProgramAsset->Serialize();
        shaderProgramAsset->ForceDeserialize();
        geometryShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void ShaderProgramDisplay::RenderFragmentShader(ShaderProgram& program) noexcept {
    ImGui::BeginDisabled();
    if (program.HasFragmentShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_MANDATORY_COLOR);
    }
    Begin("Fragment Shader");
    ImGui::InputText("###fragment_shader_text", fragmentShaderText.data(), fragmentShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::ShaderType::Fragment) {
                    assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.FragmentShader);
                    program.FragmentShader = data;
                    assets->TryRegisterDependencyBetween(shaderProgramAsset->ID(), data);
                    shaderProgramAsset->Serialize();
                    shaderProgramAsset->ForceDeserialize();
                    fragmentShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor();
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    ImGui::PushFont(boldFont);
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .80f, .10f, .15f, 1 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .94f, .51f, .55f, 1 });
    if (ImGui::Button("X", buttonSize)) {
        assets->TryDeleteDependencyBetween(shaderProgramAsset->ID(), program.FragmentShader);
        program.FragmentShader = 0;
        shaderProgramAsset->Serialize();
        shaderProgramAsset->ForceDeserialize();
        fragmentShaderText = NOT_SET_MANDATORY_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void ShaderProgramDisplay::End() noexcept {
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    ImGui::Columns(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
}