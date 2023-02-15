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
#include "Colors.hpp"
#include "ComponentUI.hpp"
#include "ImGuiExtensions.hpp"
#include "UserDefinedComponentStorage.hpp"

Observer::Observer(GUI& gui) noexcept :
    gui(gui) {}

bool Observer::Begin(Scene* scene) {
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
    bool visible = ImGui::Begin(title.c_str());

    return visible;
}

void Observer::Render(Scene& scene) {
    DisplayTargetRenderer::Render(*this, scene);
}

void Observer::End() {
    ImGui::End();
    ImGui::PopID();
}

void Observer::ResetDisplayTarget() { displayTarget = std::monostate{}; }

// Inner-struct DisplayTargetRenderer
void Observer::DisplayTargetRenderer::Render(Observer& observer, Scene& scene) {
    std::visit(overloaded::lambda {
        [](std::monostate) {
            HandleTargetWhenEmpty();
        },
        [&observer, &scene](Entity) {
            HandleTargetWhenEntity(observer, scene, observer.gui.get().sh.selectedEntity);
        },
        [&observer](FNode* file) {
            HandleTargetWhenFile(observer, *file);
        }
    }, observer.displayTarget);
}

void Observer::DisplayTargetRenderer::HandleTargetWhenEmpty() {
    static constexpr const char text[] = "Nothing to display here :)";
    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 txtSize = ImGui::CalcTextSize(text);
    for (int i = 0; i < 5; i++) {
        ImGui::SameLine((size.x - txtSize.x) / 2, 0);
        ImGui::Text(text);
        ImGui::NewLine();
    }
}
void Observer::DisplayTargetRenderer::HandleTargetWhenEntity(const Observer& observer, Scene& scene, const Entity entt) {
    const auto& idComponent = scene.GetComponent<IDComponent>(entt);
    ComponentUI::RenderIDComponent(observer, idComponent);

    const auto& transformComponent = scene.GetComponent<TransformComponent>(entt);
    ComponentUI::RenderTransformComponent(observer, transformComponent);

    if (scene.HasComponent<ParentComponent>(entt)) {
        const auto& parentComponent = scene.GetComponent<ParentComponent>(entt);
        ComponentUI::RenderParentComponent(observer, parentComponent);
    }

    if (scene.HasComponent<ChildComponent>(entt)) {
        const auto& childComponent = scene.GetComponent<ChildComponent>(entt);
        ComponentUI::RenderChildComponent(observer, childComponent);
    }

    if (scene.HasComponent<OrthoCameraComponent>(entt)) {
        const auto& orthoCameraComponent = scene.GetComponent<OrthoCameraComponent>(entt);
        ComponentUI::RenderOrthoCameraComponent(observer, orthoCameraComponent);
    }

    if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
        const auto& perspectiveCameraComponent = scene.GetComponent<PerspectiveCameraComponent>(entt);
        ComponentUI::RenderPerspectiveCameraComponent(observer, perspectiveCameraComponent);
    }

    if (scene.HasComponent<UserDefinedComponentStorage>(entt)) {
        const auto& storageComponent = scene.GetComponent<UserDefinedComponentStorage>(entt);
        ComponentUI::RenderUserDefinedComponentStorage(observer, storageComponent);
    }
}
void Observer::DisplayTargetRenderer::HandleTargetWhenFile(const Observer& observer, FNode& file) {
    GUI& gui = observer.gui.get();

    static const ImVec2 iconSize{ 60.0f, 60.0f };
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, iconSize.x * 1.25f);
    ImGui::PushFont(gui.GetFontBold());

    ImGui::PushStyleColor(ImGuiCol_Button, {});
    auto prePos = ImGui::GetCursorPos();
    auto& meta{ gui.GetMetaInfoOf(file) };
    if (ImGui::ImageButton(meta.GetSVGIcon(), iconSize)) {
        ImGui::OpenPopup("assetIconCombo");
    }
    auto afterPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos({ prePos.x, prePos.y + iconSize.y });
    RenderIconChangePopup(file, meta);
    ImGui::SetCursorPos(afterPos);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::NextColumn();

    ImGui::Text(file.Name().c_str());

    ImGui::PopFont();
    ImGui::PushFont(gui.GetFont());

    std::string path = file.Path().string();
    ImGui::Text(std::string("Path: ROOT").append(sizeof(char), static_cast<char>(std::filesystem::path::preferred_separator)).append(path).c_str());
    std::string absolutePath = file.AbsolutePath().string();
    ImGui::Text(std::string("Absolute Path: ").append(absolutePath).c_str());

    ImGui::PopFont();

    ImGui::Columns(1);
    ImGui::Separator();

    if (file.IsDirectory()) {
        RenderFolderView(observer, file);
    } else {
        AssetHandle h = gui.CORE->Assets()->FindAssetAt(file);
        if (h.HasValue()) {
            RenderAssetView(observer, h);
        }
    }
}
void Observer::DisplayTargetRenderer::RenderIconChangePopup(const FNode& file, MetaAssetInfo& meta) {
    if (ImGui::BeginPopup("assetIconCombo")) {
        const FileIcons::ElementType *begin{ nullptr }, *end{ nullptr };
        if (file.IsDirectory()) {
            auto& items = FileIcons::GetAllDirectoryIcons();
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsSceneFile(file)) {
            auto& items = FileIcons::GetAllSceneIcons();
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsComponentDefinitionFile(file)) {
            auto& items = FileIcons::GetAllComponentIcons();
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsTextureFile(file)) {
            auto& items = FileIcons::GetAllTextureIcons();
            begin = &items.front();
            end = &items.back() + 1;
        } else {
            auto& items = FileIcons::GetAllRegularFileIcons();
            begin = &items.front();
            end = &items.back() + 1;
        }

        assert(begin);
        assert(end);
        auto selectedItem = begin + meta.icon_index;
        for (auto current = begin; current != end; current++) {
            const bool isSelected{ current == selectedItem };
            if (ImGui::Selectable(current->first.c_str(), isSelected)) {
                selectedItem = current;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        meta.icon_index = selectedItem - begin;
        meta.fa_icon = selectedItem->first.c_str();
        meta.svg_icon_key = selectedItem->second;
        ImGui::EndPopup();
    }
}

void Observer::DisplayTargetRenderer::RenderFolderView(const Observer& observer, FNode& folder) {
    assert(folder.IsDirectory());
    GUI& gui = observer.gui.get();

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

        for (auto& child : folder.Children()) {
            if (!child.IsDirectory() && !gui.CORE->Assets()->IsAssetExistsAt(child)) { continue; }

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
void Observer::DisplayTargetRenderer::RenderAssetView(const Observer& observer, AssetHandle h) {
    if (h->IsScene()) {
        RenderSceneView(observer, h);
    } else if (h->IsComponentDefinition()) {
        RenderComponentDefinitionView(observer, h);
    } else if (h->IsTexture()) {
        RenderTextureView(observer, h);
    } else {
        RenderTextView(observer, h);
    }
}
void Observer::DisplayTargetRenderer::RenderSceneView(const Observer& observer, AssetHandle h) {
    assert(h->IsScene());

    Scene& scene = h->DataAs<Scene>();

    SceneDisplay::RenderEntities(observer, scene);
    SceneDisplay::RenderSystems(observer, scene);

}
void Observer::DisplayTargetRenderer::RenderComponentDefinitionView(const Observer& observer, AssetHandle h) {
    assert(h->IsComponentDefinition());

    ComponentDefinitionDisplay::RenderMessagesTable(h);

    if (h->HasDeserializedData()) {
        if (!h->HasErrorMessages()) {
            const auto& component = h->DataAs<Component>();
            ComponentDefinitionDisplay::RenderFields(component);
            ImGui::Separator();
            ComponentDefinitionDisplay::RenderSourceCode(component);
        }
    } else {
        ImGui::Text("Component Definition is not deserialized...");
    }

    static int extraPadding = 16;
    float lineHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2;
    if (ImGui::GetContentRegionAvail().y > 34.0f) {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - lineHeight - extraPadding);
    }

    if (ImGui::Button("Refresh", { ImGui::GetContentRegionAvail().x, 0 })) {
        h->ForceDeserialize();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("Forces deserialization on this component definition. All data in RAM is purged, and new data is read from disk. This operation will cause re-instantiation of user defined components without loss of data.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void Observer::DisplayTargetRenderer::RenderTextureView(const Observer& observer, AssetHandle h) {
    assert(h->IsTexture());
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

    if (h->HasDeserializedData()) {
        const auto& tex = h->DataAs<Texture>();
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
        h->ForceDeserialize();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("Forces deserialization on this texture. All data in RAM/VRAM is purged, and new data is read from disk.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void Observer::DisplayTargetRenderer::RenderTextView(const Observer& observer, AssetHandle h) {
    if (!h->File().HasContent()) {
        h->File().ReadContent();
    }
    std::string content{ h->File().Content() };

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

// Inner-struct SceneDisplay
void Observer::SceneDisplay::RenderEntities(const Observer& observer, Scene& scene) {
    GUI& gui = observer.gui;

    auto flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("entities", 1, flags)) {
        ImGui::TableSetupColumn("Entities", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (Entity entt : scene.GetAllEntites()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            std::string label = std::to_string(entt).append("\t\t(").append(scene.GetComponent<IDComponent>(entt).GetTagRef()).append(")");
            if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
                std::string nameWithIcon{};

                nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(IDComponent)) + nameof(IDComponent);
                ImGui::Text(nameWithIcon.c_str());

                nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(TransformComponent)) + nameof(TransformComponent);
                ImGui::Text(nameWithIcon.c_str());
                if (scene.HasComponent<ParentComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(ParentComponent)) + nameof(ParentComponent);
                    ImGui::Text(nameWithIcon.c_str());
                }
                if (scene.HasComponent<ChildComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(ChildComponent)) + nameof(ChildComponent);
                    ImGui::Text(nameWithIcon.c_str());
                }
                if (scene.HasComponent<OrthoCameraComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(OrthoCameraComponent)) + nameof(OrthoCameraComponent);
                    ImGui::Text(nameWithIcon.c_str());
                }
                if (scene.HasComponent<PerspectiveCameraComponent>(entt)) {
                    nameWithIcon = ComponentIcons::FindIconByComponentName(nameof(PerspectiveCameraComponent)) + nameof(PerspectiveCameraComponent);
                    ImGui::Text(nameWithIcon.c_str());
                }
                if (scene.HasComponent<UserDefinedComponentStorage>(entt)) {
                    auto& udcs{ scene.GetComponent<UserDefinedComponentStorage>(entt) };
                    for (const auto& [name, instance] : udcs.Components()) {
                        if (!instance.HasError()) {
                            AssetHandle cmpAsset{ gui.CORE->Assets()->FindAsset(instance.ComponentAssetID()) };
                            nameWithIcon = ComponentIcons::FindIconByComponentName(name) + name;
                            ImGui::Text(nameWithIcon.c_str());
                        } else {
                            ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.5f, 0.1f, 1.0f });

                            nameWithIcon = ComponentIcons::FindIconForInstantiationError(instance.GetError()) + name;
                            ImGui::Text(nameWithIcon.c_str());
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip(instance.ErrorString().data());
                            }

                            ImGui::PopStyleColor();
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }
}
void Observer::SceneDisplay::RenderSystems(const Observer& observer, const Scene& scene) {
    auto flags = ImGuiTableFlags_RowBg |
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

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
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

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
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

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
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

    TextEditor.Render("##", true, { 0.0f, -32.0f }, false);

    ImGui::EndTable();
}