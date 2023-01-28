#include "Observer.hpp"

#include <string>
#include <sstream>

#include <imgui.h>
#include <imgInspect.h>

#include <nameof.hpp>
#include <prettify.hpp>
#include <FormatBytes.hpp>

#include <Scene.hpp>
#include <IDComponent.hpp>
#include <TransformComponent.hpp>
#include <ParentComponent.hpp>
#include <ChildComponent.hpp>
#include <CameraComponent.hpp>
#include <Assets.hpp>

#include <ComponentDeserializer.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "ComponentUI.hpp"
#include "ImGuiExtensions.hpp"
#include "UserDefinedComponentStorage.hpp"

Observer::Observer(GUI& gui) noexcept :
    gui(gui) {}

void Observer::Begin(Scene* scene) {
    GUI& gui = this->gui;
    ImGui::PushID(GUI::OBSERVER_TITLE);
    std::string title(WindowIcons::OBSERVER_WINDOW_ICON);
    title.append(GUI::OBSERVER_TITLE);
    std::visit(overloaded::lambda {
        [](std::monostate) { /* skip */ },
        [&scene, &title](Entity entt) {
            if (scene && entt != NULL_ENTT) {
                title.append(" - ");
                title.append(scene->GetComponent<IDComponent>(entt).GetTag());
            }
        },
        [&title](FNode* file) {
            title.append(" - ");
            title.append(file->Name());
        }
    }, displayTarget);

    title.append(GUI::OBSERVER_ID);
    ImGui::Begin(title.c_str());
}

void Observer::Render(Scene& scene) {
    GUI& gui = this->gui;
    std::visit(overloaded::lambda{
        [](std::monostate) {
            const char* text = "Nothing to display here :)";
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImVec2 txtSize = ImGui::CalcTextSize(text);
            for (int i = 0; i < 5; i++) {
                ImGui::SameLine((size.x - txtSize.x) / 2, 0);
                ImGui::Text(text);
                ImGui::NewLine();
            }
        },
        [this, &scene, &gui](Entity) { RenderComponents(scene, gui.sh.selectedEntity); },
        [this, &gui](FNode* file) {
            static const ImVec2 iconSize{ 64.0f, 64.0f };
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, iconSize.x * 1.25f);
            ImGui::PushFont(gui.GetFontBold());

            ImGui::Image(gui.FindIconForFileType(*file), iconSize);

            ImGui::SameLine();
            ImGui::NextColumn();

            auto name = static_cast<const FNode*>(file)->Name().c_str();
            ImGui::Text(name);

            ImGui::PopFont();
            ImGui::PushFont(gui.GetFont());

            std::string path = file->Path().string();
            ImGui::Text(std::string("Path: ROOT").append(sizeof(char), static_cast<char>(std::filesystem::path::preferred_separator)).append(path).c_str());
            std::string absolutePath = file->AbsolutePath().string();
            ImGui::Text(std::string("Absolute Path: ").append(absolutePath).c_str());

            ImGui::PopFont();

            ImGui::Columns(1);
            ImGui::Separator();

            if (file->IsDirectory()) {
                RenderFolderView(file);
            } else {
                AssetHandle h = gui.openProject->Assets().FindAssetAt(*file);
                if (h.HasValue()) {
                    RenderAssetView(h);
                }
            }
        }
    }, displayTarget);
}

void Observer::End() {
    ImGui::End();
    ImGui::PopID();
}

void Observer::RenderComponents(Scene& scene, const Entity entt) {
    const auto& idComponent = scene.GetComponent<IDComponent>(entt);
    ComponentUI::RenderIDComponent(*this, idComponent);

    const auto& transformComponent = scene.GetComponent<TransformComponent>(entt);
    ComponentUI::RenderTransformComponent(*this, transformComponent);

    if (scene.HasComponent<ParentComponent>(entt)) {
        const auto& parentComponent = scene.GetComponent<ParentComponent>(entt);
        ComponentUI::RenderParentComponent(*this, parentComponent);
    }

    if (scene.HasComponent<ChildComponent>(entt)) {
        const auto& childComponent = scene.GetComponent<ChildComponent>(entt);
        ComponentUI::RenderChildComponent(*this, childComponent);
    }

    if (scene.HasComponent<OrthoCameraComponent>(entt)) {
        const auto& orthoCameraComponent = scene.GetComponent<OrthoCameraComponent>(entt);
        ComponentUI::RenderOrthoCameraComponent(*this, orthoCameraComponent);
    }

    if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
        const auto& perspectiveCameraComponent = scene.GetComponent<PerspectiveCameraComponent>(entt);
        ComponentUI::RenderPerspectiveCameraComponent(*this, perspectiveCameraComponent);
    }

    if (scene.HasComponent<UserDefinedComponentStorage>(entt)) {
        const auto& storageComponent = scene.GetComponent<UserDefinedComponentStorage>(entt);
        ComponentUI::RenderUserDefinedComponentStorage(*this, storageComponent);
    }
}
void Observer::RenderFolderView(FNode* folder) {
    assert(folder->IsDirectory());
    GUI& gui = this->gui;

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_NoBordersInBodyUntilResize;

    if (ImGui::BeginTable("folderview", 3, flags)) {

        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
        ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch, 0.6f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch, 0.3f);
        ImGui::TableHeadersRow();

        for (auto& child : folder->Children()) {
            if (!child.IsDirectory() && !gui.openProject->Assets().IsAssetExistsAt(child)) { continue; }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(gui.FindIconForFileType(child, TextureSize::SMALL), { 16.0f, 16.0f });
            ImGui::TableSetColumnIndex(1);
            ImGui::Text(child.FullName().c_str());
            ImGui::TableSetColumnIndex(2);
            std::string size = FormatBytes(static_cast<float>(child.Size()));
            ImGui::Text(size.c_str());
        }
        ImGui::EndTable();
    }
}
void Observer::RenderAssetView(AssetHandle asset) {
    if (asset->IsScene()) {
        RenderSceneView(asset);
    } else if (asset->IsComponentDefinition()) {
        RenderComponentDefinitionView(asset);
    } else if (asset->IsTexture()) {
        RenderTextureView(asset);
    } else {
        RenderTextView(asset);
    }
}
void Observer::RenderSceneView(AssetHandle sceneAsset) {
    assert(sceneAsset->IsScene());

    Scene& scene = sceneAsset->DataAs<Scene>();

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("entities", 1, flags)) {
        ImGui::TableSetupColumn("Entities", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (Entity entt : scene.GetAllEntites()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            std::string label = std::to_string(entt).append("\t\t(").append(scene.GetComponent<IDComponent>(entt).GetTagRef()).append(")");
            if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
                ImGui::Text(nameof_c(IDComponent));
                ImGui::Text(nameof_c(TransformComponent));
                if (scene.HasComponent<ParentComponent>(entt)) {
                    ImGui::Text(nameof_c(ParentComponent));
                }
                if (scene.HasComponent<ChildComponent>(entt)) {
                    ImGui::Text(nameof_c(ChildComponent));
                }
                if (scene.HasComponent<OrthoCameraComponent>(entt)) {
                    ImGui::Text(nameof_c(OrthoCameraComponent));
                }
                if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
                    ImGui::Text(nameof_c(PerspectiveCameraComponent));
                }
                if (scene.HasComponent<UserDefinedComponentStorage>(entt)) {
                    ImGui::Text(nameof_c(UserDefinedComponentStorage));
                }
            }
        }
        ImGui::EndTable();
    }

    flags = ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_NoBordersInBodyUntilResize;
    if (ImGui::BeginTable("systems", 2, flags)) {
        ImGui::TableSetupColumn("Systems", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
        ImGui::TableHeadersRow();

        ImGui::EndTable();
    }
}
void Observer::RenderComponentDefinitionView(AssetHandle componentDefAsset) {
    assert(componentDefAsset->IsComponentDefinition());

    ComponentDefinitionDisplay::RenderMessagesTable(componentDefAsset);

    if (!componentDefAsset->HasErrorMessages()) {
        const auto& component = componentDefAsset->DataAs<Component>();
        ComponentDefinitionDisplay::RenderFields(component);
        ImGui::Separator();
        ComponentDefinitionDisplay::RenderSourceCode(component);
    }
}
void Observer::RenderTextView(AssetHandle textAsset) {
    textAsset->File().ReadContent();
    std::string content{ textAsset->File().DisposeContent() };

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("content", 1, flags)) {
        ImGui::TableSetupColumn("File Content", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        auto size = ImGui::GetContentRegionAvail();
        size.y -= 5; /* this "hides" the "1px vertical scroll-bar" in observer window */
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
        ImGui::InputTextMultiline("", content.data(), content.size(), size, flags);

        ImGui::EndTable();
    }
}
void Observer::RenderTextureView(AssetHandle textureAsset) {
    assert(textureAsset->IsTexture());
    /* channels */
    static bool r{ true }, g{ true }, b{ true }, a{ true };
    /* inspect params */
    static bool drawInspector{ true }, drawHistogram{ false }, drawNormals{ false };

    static int bottomContentLineCount = 3;
    float lineHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2;
    static float extraPadding = 16; /* pad by an extra amount to remove scroll bar, don't pad and see the scroll bar appear on right side */
    //ImGui::SliderFloat("label", &extraPadding, 0, 50); /* was used to test padding amount, not deleted to easily re-test in future */
    float totalBottomPadding = lineHeight * bottomContentLineCount + extraPadding;

    auto [windowWidth, windowHeight] = ImGui::GetContentRegionAvail();
    windowWidth = windowWidth - ImGui::GetStyle().FramePadding.x;
    windowHeight = windowHeight - totalBottomPadding;

    if (textureAsset->HasDeserializedData()) {
        const auto& tex = textureAsset->DataAs<Texture>();
        float w = static_cast<float>(tex.Width());
        float h = static_cast<float>(tex.Height());
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

        ImGui::Image(tex.TextureIDRaw(), { w, h }, { 0, 1 }, { 1, 0 }, { (float) r, (float) g, (float) b, (float) a }, { 1, 1, 0, 1 });

        if (drawInspector) {
            ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            ImVec2 mouseUVCoord = (ImGui::GetIO().MousePos - rc.Min) / rc.GetSize();
            mouseUVCoord.y = 1.f - mouseUVCoord.y;
            if (mouseUVCoord.x >= 0.0f &&
                mouseUVCoord.y >= 0.0f &&
                mouseUVCoord.x <= 1.0f &&
                mouseUVCoord.y <= 1.0f) {
                float w = static_cast<float>(tex.Width());
                float h = static_cast<float>(tex.Height());
                auto pixels = reinterpret_cast<const unsigned char*>(Texture::GetByteBufferOf(tex));
                ImageInspect::inspect(static_cast<int>(w), static_cast<int>(h), pixels, mouseUVCoord, { w, h }, drawNormals, drawHistogram);
            }
        }
    } else {
        ImGui::Text("Texture is not deserialized...");
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
    if (ImGui::Button("Refresh", { ImGui::GetContentRegionAvail().x, 0 })) {
        textureAsset->ForceDeserialize();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("Forces deserialization on this texture. All data in RAM/VRAM is purged, and new data is read from disk.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Observer::ResetDisplayTarget() { displayTarget = std::monostate{}; }


// Inner-struct ComponentDefinitionDisplay
void Observer::ComponentDefinitionDisplay::RenderMessagesTable(const AssetHandle componentDefAsset) {
    if(!componentDefAsset->HasErrorMessages() &&
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

    ImGui::PushStyleColor(ImGuiCol_Text, ERROR_COLOR);
    for (auto& message : componentDefAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped(m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, WARNING_COLOR);
    for (auto& message : componentDefAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped(m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, INFO_COLOR);
    for (auto& message : componentDefAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ComponentCompilerMessage& m{ std::any_cast<const ComponentCompilerMessage&>(message) };
        ImGui::TextWrapped(m.message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void Observer::ComponentDefinitionDisplay::RenderFields(const Component& componentDef) {
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
}
void Observer::ComponentDefinitionDisplay::RenderSourceCode(const Component& componentDef) {
    TextEditor.SetReadOnly(true);
    TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
    TextEditor.SetText(componentDef.declaration);

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    ImGui::BeginTable("source_code", 1, flags);
    ImGui::TableSetupColumn("Source Code", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    TextEditor.Render("##", false, { 0.0f, -10.0f }, false);

    ImGui::EndTable();
}