#include <Editor/ShaderDisplay.hpp>

#include <Engine/Shader.hpp>
#include <Engine/ShaderDeserializer.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>

ShaderDisplay::ShaderDisplay(Observer& observer) noexcept :
    observer(observer) {
    textEditorInstance.SetColorizerEnable(true);
    textEditorInstance.SetReadOnlyEnabled(true);
    textEditorInstance.SetShowWhitespaces(false);
    textEditorInstance.SetText(std::string(10, ' '));
    textEditorInstance.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

void ShaderDisplay::SetDisplayTarget(const AssetHandle shaderAssetHandle) noexcept {
    assert(shaderAssetHandle->IsShader());
    if (shaderAsset != shaderAssetHandle || assetVersion < shaderAssetHandle->Version()) {
        assetVersion = shaderAssetHandle->Version();
        shaderAsset = shaderAssetHandle;
        if (shaderAsset->HasDeserializedData() && !shaderAsset->HasErrorMessages()) {
            const auto& shader = shaderAsset->DataAs<Shader>();
            textEditorInstance.SetText(shader.SourceCode);
        }
    }
}

void ShaderDisplay::RenderMessagesTable() noexcept {
    if (!shaderAsset->HasErrorMessages() &&
        !shaderAsset->HasWarningMessages() &&
        !shaderAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Compiler Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : shaderAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.ShortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : shaderAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.ShortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : shaderAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.ShortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void ShaderDisplay::RenderFields() noexcept {
    /*
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::CollapsingHeader(componentDef.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("component_fields", 2, flags)) {
            ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& field : componentDef.fields) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(field.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(field.typeName.c_str());
            }
            ImGui::EndTable();
        }
    }
    */
}
void ShaderDisplay::RenderSourceCode() noexcept {
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    ImGui::BeginTable("source_code", 1, flags);
    ImGui::TableSetupColumn("Source Code", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    textEditorInstance.Render("###ObserverShaderSourceCodeViewer", false, { 0.0f, -32.0f });

    ImGui::EndTable();
}