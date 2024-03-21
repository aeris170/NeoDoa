#include <Editor/ComponentDefinitionDisplay.hpp>

#include <Engine/Component.hpp>
#include <Engine/ComponentDeserializer.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>

ComponentDefinitionDisplay::ComponentDefinitionDisplay(Observer& observer) noexcept :
    observer(observer) {
    textEditorInstance.SetColorizerEnable(true);
    textEditorInstance.SetReadOnlyEnabled(true);
    textEditorInstance.SetShowWhitespaces(false);
    textEditorInstance.SetText(std::string(10, ' '));
    textEditorInstance.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
}

void ComponentDefinitionDisplay::SetDisplayTarget(const AssetHandle componentDefAssetHandle) noexcept {
    assert(componentDefAssetHandle->IsComponentDefinition());
    if (componentDefAsset != componentDefAssetHandle) {
        componentDefAsset = componentDefAssetHandle;
        if (componentDefAsset->HasDeserializedData() && !componentDefAsset->HasErrorMessages()) {
            const auto& componentDef = componentDefAsset->DataAs<Component>();
            textEditorInstance.SetText(componentDef.declaration);
        }
    }
}

void ComponentDefinitionDisplay::RenderMessagesTable() noexcept {
    if (!componentDefAsset->HasErrorMessages() &&
        !componentDefAsset->HasWarningMessages() &&
        !componentDefAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Compiler Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : componentDefAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : componentDefAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : componentDefAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped("%s", m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void ComponentDefinitionDisplay::RenderFields() noexcept {
    assert(componentDefAsset->HasDeserializedData());
    assert(!componentDefAsset->HasErrorMessages());

    const auto& componentDef = componentDefAsset->DataAs<Component>();

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::CollapsingHeader(componentDef.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("component_fields", 2, flags)) {
            ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& field : componentDef.fields) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(field.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(field.typeName.c_str());
            }
            ImGui::EndTable();
        }
    }
}
void ComponentDefinitionDisplay::RenderSourceCode() noexcept {
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    ImGui::BeginTable("source_code", 1, flags);
    ImGui::TableSetupColumn("Source Code", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    textEditorInstance.Render("###ObserverComponentDefSourceCodeViewer", false, { 0.0f, -32.0f });

    ImGui::EndTable();
}