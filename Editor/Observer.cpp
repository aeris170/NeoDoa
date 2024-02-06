#include <Editor/Observer.hpp>

#include <string>
#include <sstream>

#include <imgui.h>
#include <imgInspect.h>

#include <Utility/NameOf.hpp>
#include <Utility/Prettify.hpp>
#include <Utility/FormatBytes.hpp>

#include <Engine/Scene.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/CameraComponent.hpp>
#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ComponentDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Strings.hpp>
#include <Editor/ComponentUI.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/UserDefinedComponentStorage.hpp>

Observer::Observer(GUI& gui) noexcept :
    gui(gui) {
    ComponentDefinitionDisplay::Init();
    ShaderDisplay::Init();
    ShaderProgramDisplay::Init();

    gui.Events.OnReimport                        += std::bind_front(&Observer::OnReimport,         this);
    gui.Events.OnAssetDeleted                    += std::bind_front(&Observer::OnAssetDeleted,     this);
    gui.Events.AssetManager.OnAssetFocused       += std::bind_front(&Observer::OnAssetFocused,     this);
    gui.Events.AssetManager.OnFolderFocused      += std::bind_front(&Observer::OnFolderFocused,    this);
    gui.Events.AssetManager.OnFocusLost          += std::bind_front(&Observer::OnFocusLost,        this);
    gui.Events.OnSceneClosed                     += std::bind_front(&Observer::OnSceneClosed,      this);
    gui.Events.OnEntityDeleted                   += std::bind_front(&Observer::OnEntityDeleted,    this);
    gui.Events.SceneHierarchy.OnEntitySelected   += std::bind_front(&Observer::OnEntitySelected,   this);
    gui.Events.SceneHierarchy.OnEntityDeselected += std::bind_front(&Observer::OnEntityDeselected, this);
}

bool Observer::Begin() {
    ImGui::PushID(WindowStrings::ObserverWindowName);
    bool visible = ImGui::Begin(WindowStrings::ObserverWindowTitleID);

    return visible;
}

void Observer::Render() {
    GUI& gui = this->gui;
    DisplayTargetRenderer::Render(*this);
}

void Observer::End() {
    ImGui::End();
    ImGui::PopID();
}

void Observer::SetDisplayTarget(Entity entity) {
    static std::string hypen(" - ");
    displayTarget = entity;
    renderTargetTitleText = hypen + gui.get().GetOpenScene().GetComponent<IDComponent>(entity).GetTagRef();
}
void Observer::SetDisplayTarget(FNode& file) {
    static std::string hypen(" - ");
    displayTarget = &file;
    renderTargetTitleText = hypen + file.Name().data();
}
void Observer::ResetDisplayTarget() {
    displayTarget = std::monostate{};
    renderTargetTitleText = "";
}

// Inner-struct DisplayTargetRenderer
void Observer::DisplayTargetRenderer::Render(Observer& observer) {
    std::visit(overloaded::lambda {
        [](std::monostate) {
            HandleTargetWhenEmpty();
        },
        [&observer](Entity entity) {
            GUI& gui = observer.gui.get();
            if (!gui.HasOpenScene()) { assert(false); }

            Scene& scene = gui.GetOpenScene();
            HandleTargetWhenEntity(observer, scene, entity);
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
    GUI& gui = observer.gui;

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

    ImGui::Text(file.Name().data());

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
        AssetHandle h = gui.CORE->GetAssets()->FindAssetAt(file);
        if (h.HasValue()) {
            RenderAssetView(observer, h);
        }
    }
}
void Observer::DisplayTargetRenderer::RenderIconChangePopup(const FNode& file, MetaAssetInfo& meta) {
    if (ImGui::BeginPopup("assetIconCombo")) {
        const FileIcons::ElementType *begin{ nullptr }, *end{ nullptr };
        if (file.IsDirectory()) {
            auto& items = FileIcons::DirectoryIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsSceneFile(file)) {
            auto& items = FileIcons::SceneIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsComponentDefinitionFile(file)) {
            auto& items = FileIcons::ComponentIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsShaderFile(file)) {
            auto& items = FileIcons::ShaderIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsShaderProgramFile(file)) {
            auto& items = FileIcons::ShaderProgramIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsTextureFile(file)) {
            auto& items = FileIcons::TextureIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else {
            auto& items = FileIcons::RegularFileIcons;
            begin = &items.front();
            end = &items.back() + 1;
        }

        assert(begin);
        assert(end);
        auto selectedItem = begin + meta.icon_index;
        for (auto current = begin; current != end; current++) {
            const bool isSelected{ current == selectedItem };
            if (ImGui::Selectable(current->first, isSelected)) {
                selectedItem = current;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        meta.icon_index = selectedItem - begin;
        meta.fa_icon = selectedItem->first;
        meta.svg_icon_key = selectedItem->second;
        ImGui::EndPopup();
    }
}

void Observer::DisplayTargetRenderer::RenderFolderView(const Observer& observer, FNode& folder) {
    assert(folder.IsDirectory());
    const GUI& gui = observer.gui;

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
            if (!child.IsDirectory() && !gui.CORE->GetAssets()->IsAssetExistsAt(child)) { continue; }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(gui.FindIconForFileType(child, TextureSize::SMALL), { 16.0f, 16.0f });
            ImGui::TableSetColumnIndex(1);
            ImGui::Text(child.FullName().data());
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
    } else if (h->IsShader()) {
        RenderShaderView(observer, h);
    } else if (h->IsShaderProgram()) {
        RenderShaderProgramView(observer, h);
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

    ComponentDefinitionDisplay::SetDisplayTarget(h);
    ComponentDefinitionDisplay::RenderMessagesTable();

    if (h->HasDeserializedData()) {
        if (!h->HasErrorMessages()) {
            ComponentDefinitionDisplay::RenderFields();
            ImGui::Separator();
            ComponentDefinitionDisplay::RenderSourceCode();
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
void Observer::DisplayTargetRenderer::RenderShaderView(const Observer& observer, AssetHandle h) {
    assert(h->IsShader());

    ShaderDisplay::SetDisplayTarget(h);
    ShaderDisplay::RenderMessagesTable();

    if (h->HasDeserializedData()) {
        if (!h->HasErrorMessages()) {
            ShaderDisplay::RenderFields();
            ImGui::Separator();
            ShaderDisplay::RenderSourceCode();
        }
    } else {
        ImGui::Text("Shader is not deserialized...");
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
        ImGui::TextUnformatted("Forces deserialization on this shader. All data in VRAM is purged, and new data is allocated.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void Observer::DisplayTargetRenderer::RenderShaderProgramView(const Observer& observer, AssetHandle h) {
    assert(h->IsShaderProgram());

    ShaderProgramDisplay::SetDisplayTarget(*observer.gui.get().CORE->GetAssets(), h);
    ShaderProgramDisplay::RenderMessagesTable();
    ImGui::Separator();
    if (h->HasDeserializedData()) {
        ShaderProgramDisplay::RenderShaders();
    } else {
        ImGui::Text("Shader Program is not deserialized...");
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
        ImGui::TextUnformatted("Forces deserialization on this shader program. All data in VRAM is purged, and new data is allocated.");
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
                            AssetHandle cmpAsset{ gui.CORE->GetAssets()->FindAsset(instance.ComponentAssetID()) };
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
void Observer::ComponentDefinitionDisplay::Init() {
    TextEditor.SetColorizerEnable(true);
    TextEditor.SetReadOnlyEnabled(true);
    TextEditor.SetShowWhitespaces(false);
    TextEditor.SetText(std::string(10, ' '));
    TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
}

void Observer::ComponentDefinitionDisplay::SetDisplayTarget(const AssetHandle componentDefAsset) {
    assert(componentDefAsset->IsComponentDefinition());
    if (ComponentDefAsset != componentDefAsset) {
        ComponentDefAsset = componentDefAsset;
        if (ComponentDefAsset->HasDeserializedData() && !ComponentDefAsset->HasErrorMessages()) {
            const auto& componentDef = ComponentDefAsset->DataAs<Component>();
            TextEditor.SetText(componentDef.declaration);
        }
    }
}

void Observer::ComponentDefinitionDisplay::RenderMessagesTable() {
    assert(ComponentDefAsset.HasValue());
    if(!ComponentDefAsset->HasErrorMessages() &&
        !ComponentDefAsset->HasWarningMessages() &&
        !ComponentDefAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Compiler Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : ComponentDefAsset->ErrorMessages()) {
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
    for (auto& message : ComponentDefAsset->WarningMessages()) {
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
    for (auto& message : ComponentDefAsset->InfoMessages()) {
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
void Observer::ComponentDefinitionDisplay::RenderFields() {
    assert(ComponentDefAsset->HasDeserializedData());
    assert(!ComponentDefAsset->HasErrorMessages());

    const auto& componentDef = ComponentDefAsset->DataAs<Component>();

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
void Observer::ComponentDefinitionDisplay::RenderSourceCode() {
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    ImGui::BeginTable("source_code", 1, flags);
    ImGui::TableSetupColumn("Source Code", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    TextEditor.Render("###ObserverComponentDefSourceCodeViewer", false, { 0.0f, -32.0f });

    ImGui::EndTable();
}

// Inner-struct ShaderDisplay
void Observer::ShaderDisplay::Init() {
    TextEditor.SetColorizerEnable(true);
    TextEditor.SetReadOnlyEnabled(true);
    TextEditor.SetShowWhitespaces(false);
    TextEditor.SetText(std::string(10, ' '));
    TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

void Observer::ShaderDisplay::SetDisplayTarget(const AssetHandle shaderAsset) {
    assert(shaderAsset->IsShader());
    if (ShaderAsset != shaderAsset) {
        ShaderAsset = shaderAsset;
        if (ShaderAsset->HasDeserializedData() && !ShaderAsset->HasErrorMessages()) {
            const auto& shader = ShaderAsset->DataAs<Shader>();
            TextEditor.SetText(shader.SourceCode);
        }
    }
}

void Observer::ShaderDisplay::RenderMessagesTable() {
    assert(ShaderAsset.HasValue());
    if (!ShaderAsset->HasErrorMessages() &&
        !ShaderAsset->HasWarningMessages() &&
        !ShaderAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Compiler Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : ShaderAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped(m.shortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : ShaderAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped(m.shortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : ShaderAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const ShaderCompilerMessage& m{ std::any_cast<const ShaderCompilerMessage&>(message) };
        ImGui::TextWrapped(m.shortMessage.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void Observer::ShaderDisplay::RenderFields() {
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
void Observer::ShaderDisplay::RenderSourceCode() {
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    ImGui::BeginTable("source_code", 1, flags);
    ImGui::TableSetupColumn("Source Code", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    TextEditor.Render("###ObserverShaderSourceCodeViewer", false, { 0.0f, -32.0f });

    ImGui::EndTable();
}

// Inner-struct ShaderProgramDisplay
void Observer::ShaderProgramDisplay::Init() {
    VertexShaderText = std::string(128, 0);
    TessCtrlShaderText = std::string(128, 0);
    TessEvalShaderText = std::string(128, 0);
    GeometryShaderText = std::string(128, 0);
    FragmentShaderText = std::string(128, 0);
}
void Observer::ShaderProgramDisplay::SetDisplayTarget(Assets& assets, const AssetHandle shaderProgramAsset) {
    Observer::ShaderProgramDisplay::assets = &assets;
    assert(shaderProgramAsset->IsShaderProgram() && shaderProgramAsset.HasValue());
    if (ShaderProgramAsset != shaderProgramAsset) {
        ShaderProgramAsset = shaderProgramAsset;
        const ShaderProgram& program = ShaderProgramAsset->DataAs<ShaderProgram>();

        if (program.HasVertexShader()) {
            UUID uuid = program.VertexShader;
            VertexShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            VertexShaderText = NOT_SET_MANDATORY_TEXT;
        }

        if (program.HasTessellationControlShader()) {
            UUID uuid = program.TessellationControlShader;
            TessCtrlShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            TessCtrlShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasTessellationEvaluationShader()) {
            UUID uuid = program.TessellationEvaluationShader;
            TessEvalShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            TessEvalShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasGeometryShader()) {
            UUID uuid = program.GeometryShader;
            GeometryShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            GeometryShaderText = NOT_SET_OPTIONAL_TEXT;
        }

        if (program.HasFragmentShader()) {
            UUID uuid = program.FragmentShader;
            FragmentShaderText = std::format("{} (UUID: {})", assets.FindAsset(uuid)->File().Name(), std::to_string(uuid));
        } else {
            FragmentShaderText = NOT_SET_MANDATORY_TEXT;
        }
    }
}

void Observer::ShaderProgramDisplay::RenderMessagesTable() {
    assert(ShaderProgramAsset.HasValue());
    if (!ShaderProgramAsset->HasErrorMessages() &&
        !ShaderProgramAsset->HasWarningMessages() &&
        !ShaderProgramAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("linker_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Linker Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : ShaderProgramAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped(m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : ShaderProgramAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped(m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : ShaderProgramAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped(m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void Observer::ShaderProgramDisplay::RenderShaders() {
    ShaderProgram& program = ShaderProgramAsset->DataAs<ShaderProgram>();
    RenderVertexShader(program);
    RenderTessCtrlShader(program);
    RenderTessEvalShader(program);
    RenderGeometryShader(program);
    RenderFragmentShader(program);
}

void Observer::ShaderProgramDisplay::Begin(std::string_view label) {
    float w = ImGui::GetContentRegionAvail().x;

    ImGui::PushID(label.data());
    ImGui::Columns(2, nullptr, false);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0 });
    ImGui::SetColumnWidth(0, 240);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f + 3);
    ImGui::TextDisabled(label.data());
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
void Observer::ShaderProgramDisplay::RenderVertexShader(ShaderProgram& program) {
    ImGui::BeginDisabled();
    if (program.HasVertexShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_MANDATORY_COLOR);
    }
    Begin("Vertex Shader");
    ImGui::InputText("###vertex_shader_text", VertexShaderText.data(), VertexShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::Type::Vertex) {
                    program.VertexShader = data;
                    ShaderProgramAsset->Serialize();
                    ShaderProgramAsset->ForceDeserialize();
                    VertexShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
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
        program.VertexShader = 0;
        ShaderProgramAsset->Serialize();
        ShaderProgramAsset->ForceDeserialize();
        VertexShaderText = NOT_SET_MANDATORY_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void Observer::ShaderProgramDisplay::RenderTessCtrlShader(ShaderProgram& program) {
    ImGui::BeginDisabled();
    if (program.HasTessellationControlShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Tessellation Control Shader");
    ImGui::InputText("###tess_ctrl_shader_text", TessCtrlShaderText.data(), TessCtrlShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::Type::TessellationControl) {
                    program.TessellationControlShader = data;
                    ShaderProgramAsset->Serialize();
                    ShaderProgramAsset->ForceDeserialize();
                    TessCtrlShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
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
        program.TessellationControlShader = 0;
        ShaderProgramAsset->Serialize();
        ShaderProgramAsset->ForceDeserialize();
        TessCtrlShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void Observer::ShaderProgramDisplay::RenderTessEvalShader(ShaderProgram& program) {
    ImGui::BeginDisabled();
    if (program.HasTessellationEvaluationShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Tessellation Evaluation Shader");
    ImGui::InputText("###tess_eval_shader_text", TessEvalShaderText.data(), TessEvalShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::Type::TessellationEvaluation) {
                    program.TessellationEvaluationShader = data;
                    ShaderProgramAsset->Serialize();
                    ShaderProgramAsset->ForceDeserialize();
                    TessEvalShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
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
        program.TessellationEvaluationShader = 0;
        ShaderProgramAsset->Serialize();
        ShaderProgramAsset->ForceDeserialize();
        TessEvalShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void Observer::ShaderProgramDisplay::RenderGeometryShader(ShaderProgram& program) {
    ImGui::BeginDisabled();
    if (program.HasGeometryShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_OPTIONAL_COLOR);
    }
    Begin("Geometry Shader");
    ImGui::InputText("###geometry_text", GeometryShaderText.data(), GeometryShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::Type::Geometry) {
                    program.GeometryShader = data;
                    ShaderProgramAsset->Serialize();
                    ShaderProgramAsset->ForceDeserialize();
                    GeometryShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
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
        program.GeometryShader = 0;
        ShaderProgramAsset->Serialize();
        ShaderProgramAsset->ForceDeserialize();
        GeometryShaderText = NOT_SET_OPTIONAL_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void Observer::ShaderProgramDisplay::RenderFragmentShader(ShaderProgram& program) {
    ImGui::BeginDisabled();
    if (program.HasFragmentShader()) {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, SET_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, NOT_SET_MANDATORY_COLOR);
    }
    Begin("Fragment Shader");
    ImGui::InputText("###fragment_shader_text", FragmentShaderText.data(), FragmentShaderText.capacity(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
            UUID data = *(const UUID*) payload->Data;
            AssetHandle handle = assets->FindAsset(data);
            assert(handle.HasValue());
            if (handle->IsShader()) {
                if (handle->DataAs<Shader>().Type == Shader::Type::Fragment) {
                    program.FragmentShader = data;
                    ShaderProgramAsset->Serialize();
                    ShaderProgramAsset->ForceDeserialize();
                    FragmentShaderText = std::format("{} (UUID: {})", assets->FindAsset(data)->File().Name(), std::to_string(data));
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
        program.FragmentShader = 0;
        ShaderProgramAsset->Serialize();
        ShaderProgramAsset->ForceDeserialize();
        FragmentShaderText = NOT_SET_MANDATORY_TEXT;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    End();
}
void Observer::ShaderProgramDisplay::End() {
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    ImGui::Columns(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
}

void Observer::OnReimport(Assets& assets) {
    if (!std::holds_alternative<FNode*>(displayTarget)) { return; }
    //TODO
}
void Observer::OnAssetDeleted(AssetHandle asset) {
    if (std::holds_alternative<FNode*>(displayTarget) &&
        &asset->File() == std::get<FNode*>(displayTarget)) {
        ResetDisplayTarget();
    }
}
void Observer::OnAssetFocused(AssetHandle asset) {
    SetDisplayTarget(asset->File());
}
void Observer::OnFolderFocused(FNode& folder) {
    SetDisplayTarget(folder);
}
void Observer::OnFocusLost() {
    ResetDisplayTarget();
}
void Observer::OnSceneClosed() {
    if (std::holds_alternative<Entity>(displayTarget)) {
        ResetDisplayTarget();
    }
}
void Observer::OnEntityDeleted(Entity entity) {
    if (std::holds_alternative<Entity>(displayTarget)) {
        if (entity == std::get<Entity>(displayTarget)) {
            ResetDisplayTarget();
        }
    }
}
void Observer::OnEntitySelected(Entity entity) {
    SetDisplayTarget(entity);
}
void Observer::OnEntityDeselected() {
    if (std::holds_alternative<Entity>(displayTarget)) {
        ResetDisplayTarget();
    }
}
