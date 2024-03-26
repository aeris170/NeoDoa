#include <Editor/DisplayTargetRenderer.hpp>

#include <imgInspect.h>

#include <Utility/FormatBytes.hpp>

#include <Engine/IDComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/CameraComponent.hpp>
#include <Engine/TransformComponent.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ComponentUI.hpp>
#include <Editor/MetaAssetInfo.hpp>
#include <Editor/UserDefinedComponentStorage.hpp>

DisplayTargetRenderer::DisplayTargetRenderer(Observer& observer) noexcept :
    observer(observer),
    sceneDisplay(observer),
    componentDefinitionDisplay(observer),
    shaderDisplay(observer),
    shaderProgramDisplay(observer),
    materialDisplay(observer),
    samplerDisplay(observer) {
    GUI& gui = observer.gui;
    gui.Events.OnProjectUnloaded                 += std::bind_front(&DisplayTargetRenderer::OnProjectUnloaded,  this);
    gui.Events.OnReimport                        += std::bind_front(&DisplayTargetRenderer::OnReimport,         this);
    gui.Events.OnAssetDeleted                    += std::bind_front(&DisplayTargetRenderer::OnAssetDeleted,     this);
    gui.Events.AssetManager.OnAssetFocused       += std::bind_front(&DisplayTargetRenderer::OnAssetFocused,     this);
    gui.Events.AssetManager.OnFolderFocused      += std::bind_front(&DisplayTargetRenderer::OnFolderFocused,    this);
    gui.Events.AssetManager.OnFocusLost          += std::bind_front(&DisplayTargetRenderer::OnFocusLost,        this);
    gui.Events.OnSceneClosed                     += std::bind_front(&DisplayTargetRenderer::OnSceneClosed,      this);
    gui.Events.OnEntityDeleted                   += std::bind_front(&DisplayTargetRenderer::OnEntityDeleted,    this);
    gui.Events.SceneHierarchy.OnEntitySelected   += std::bind_front(&DisplayTargetRenderer::OnEntitySelected,   this);
    gui.Events.SceneHierarchy.OnEntityDeselected += std::bind_front(&DisplayTargetRenderer::OnEntityDeselected, this);
}

void DisplayTargetRenderer::SetDisplayTarget(Entity entity) {
    static std::string hypen(" - ");
    displayTarget = entity;
    renderTargetTitleText = hypen + observer.get().gui.get().GetOpenScene().GetComponent<IDComponent>(entity).GetTagRef();
}
void DisplayTargetRenderer::SetDisplayTarget(FNode& file) {
    static std::string hypen(" - ");
    displayTarget = &file;
    renderTargetTitleText = hypen + file.Name().data();
}
void DisplayTargetRenderer::ResetDisplayTarget() {
    displayTarget = std::monostate{};
    renderTargetTitleText = "";
}

void DisplayTargetRenderer::Render() {
    std::visit(overloaded::lambda{
        [this](std::monostate) {
            HandleTargetWhenEmpty();
        },
        [this](Entity entity) {
            GUI& gui = observer.get().gui.get();
            if (!gui.HasOpenScene()) { assert(false); }

            Scene& scene = gui.GetOpenScene();
            HandleTargetWhenEntity(scene, entity);
        },
        [this](FNode* file) {
            HandleTargetWhenFile(*file);
        }
    }, displayTarget);
}

void DisplayTargetRenderer::HandleTargetWhenEmpty() {
    static constexpr const char text[] = "Nothing to display here :)";
    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 txtSize = ImGui::CalcTextSize(text);
    for (int i = 0; i < 5; i++) {
        ImGui::SameLine((size.x - txtSize.x) / 2, 0);
        ImGui::Text(text);
        ImGui::NewLine();
    }
}
void DisplayTargetRenderer::HandleTargetWhenEntity(Scene& scene, const Entity entt) {
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
void DisplayTargetRenderer::HandleTargetWhenFile(FNode& file) {
    GUI& gui = observer.get().gui;

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

    ImGui::TextUnformatted(file.Name().data());

    ImGui::PopFont();
    ImGui::PushFont(gui.GetFont());

    std::string path = file.Path().string();
    ImGui::TextUnformatted(std::string("Path: ROOT").append(sizeof(char), static_cast<char>(std::filesystem::path::preferred_separator)).append(path).c_str());
    std::string absolutePath = file.AbsolutePath().string();
    ImGui::TextUnformatted(std::string("Absolute Path: ").append(absolutePath).c_str());

    ImGui::PopFont();

    ImGui::Columns(1);
    ImGui::Separator();

    if (file.IsDirectory()) {
        RenderFolderView(file);
    } else {
        AssetHandle h = gui.CORE->GetAssets()->FindAssetAt(file);
        if (h.HasValue()) {
            RenderAssetView(h);
        }
    }
}
void DisplayTargetRenderer::RenderIconChangePopup(const FNode& file, MetaAssetInfo& meta) {
    if (ImGui::BeginPopup("assetIconCombo")) {
        const FileIcons::ElementType* begin{ nullptr }, * end{ nullptr };
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
        } else if (Assets::IsMaterialFile(file)) {
            auto& items = FileIcons::MaterialIcons;
            begin = &items.front();
            end = &items.back() + 1;
        } else if (Assets::IsSamplerFile(file)) {
            auto& items = FileIcons::SamplerIcons;
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

void DisplayTargetRenderer::RenderFolderView(FNode& folder) {
    assert(folder.IsDirectory());
    GUI& gui = observer.get().gui;

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
            ImGui::Image(gui.GetMetaInfoOf(child).GetSVGIcon(TextureSize::SMALL), { 16.0f, 16.0f });
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(child.FullName().data());
            ImGui::TableSetColumnIndex(2);
            std::string size = FormatBytes(static_cast<float>(child.Size()));
            ImGui::TextUnformatted(size.c_str());
        }
        ImGui::EndTable();
    }
}
void DisplayTargetRenderer::RenderAssetView(AssetHandle h) {
    if (h->IsScene()) {
        RenderSceneView(h);
    } else if (h->IsComponentDefinition()) {
        RenderComponentDefinitionView(h);
    } else if (h->IsShader()) {
        RenderShaderView(h);
    } else if (h->IsShaderProgram()) {
        RenderShaderProgramView(h);
    } else if (h->IsMaterial()) {
        RenderMaterialView(h);
    } else if (h->IsSampler()) {
        RenderSamplerView(h);
    } else if (h->IsTexture()) {
        RenderTextureView(h);
    } else {
        RenderTextView(h);
    }
}
void DisplayTargetRenderer::RenderSceneView(AssetHandle h) {
    assert(h->IsScene());

    const Scene& scene = h->DataAs<Scene>();

    sceneDisplay.RenderEntities(scene);
    sceneDisplay.RenderSystems(scene);
}
void DisplayTargetRenderer::RenderComponentDefinitionView(AssetHandle h) {
    assert(h->IsComponentDefinition());

    componentDefinitionDisplay.SetDisplayTarget(h);
    componentDefinitionDisplay.RenderMessagesTable();

    if (h->HasDeserializedData()) {
        if (!h->HasErrorMessages()) {
            componentDefinitionDisplay.RenderFields();
            ImGui::Separator();
            componentDefinitionDisplay.RenderSourceCode();
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
void DisplayTargetRenderer::RenderShaderView(AssetHandle h) {
    assert(h->IsShader());

    shaderDisplay.SetDisplayTarget(h);
    shaderDisplay.RenderMessagesTable();

    if (h->HasDeserializedData()) {
        if (!h->HasErrorMessages()) {
            shaderDisplay.RenderFields();
            ImGui::Separator();
            shaderDisplay.RenderSourceCode();
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
void DisplayTargetRenderer::RenderShaderProgramView(AssetHandle h) {
    assert(h->IsShaderProgram());

    shaderProgramDisplay.SetDisplayTarget(*observer.get().gui.get().CORE->GetAssets(), h);
    shaderProgramDisplay.RenderMessagesTable();
    ImGui::Separator();
    if (h->HasDeserializedData()) {
        shaderProgramDisplay.RenderShaders();
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
void DisplayTargetRenderer::RenderMaterialView(AssetHandle h) {
    assert(h->IsMaterial());

    materialDisplay.SetDisplayTarget(*observer.get().gui.get().CORE->GetAssets(), h);
    materialDisplay.RenderMessagesTable();
    ImGui::Separator();
    if (h->HasDeserializedData()) {
        materialDisplay.RenderProgramCombo();
        ImGui::Separator();
        materialDisplay.RenderShaderUniforms();
    } else {
        ImGui::Text("Material is not deserialized...");
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
        ImGui::TextUnformatted("Forces deserialization on material. All data in RAM is purged, and new data is allocated.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void DisplayTargetRenderer::RenderSamplerView(AssetHandle h) {
    assert(h->IsSampler());

    samplerDisplay.SetDisplayTarget(h);
    samplerDisplay.RenderMessagesTable();
    ImGui::Separator();
    if (h->HasDeserializedData() && !h->HasErrorMessages()) {
        samplerDisplay.RenderSamplerParameters();
    } else {
        ImGui::Text("Sampler is not deserialized...");
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
        ImGui::TextUnformatted("Forces deserialization on sampler. All data in RAM is purged, and new data is allocated.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void DisplayTargetRenderer::RenderTextureView(AssetHandle h) {
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
void DisplayTargetRenderer::RenderTextView(AssetHandle h) {
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

void DisplayTargetRenderer::OnProjectUnloaded() {
    ResetDisplayTarget();
}
void DisplayTargetRenderer::OnReimport(Assets& assets) {
    if (!std::holds_alternative<FNode*>(displayTarget)) { return; }
}
void DisplayTargetRenderer::OnAssetDeleted(AssetHandle asset) {
    if (std::holds_alternative<FNode*>(displayTarget) &&
        &asset->File() == std::get<FNode*>(displayTarget)) {
        ResetDisplayTarget();
    }
}
void DisplayTargetRenderer::OnAssetFocused(AssetHandle asset) {
    SetDisplayTarget(asset->File());
}
void DisplayTargetRenderer::OnFolderFocused(FNode& folder) {
    SetDisplayTarget(folder);
}
void DisplayTargetRenderer::OnFocusLost() {
    ResetDisplayTarget();
}
void DisplayTargetRenderer::OnSceneClosed() {
    if (std::holds_alternative<Entity>(displayTarget)) {
        ResetDisplayTarget();
    }
}
void DisplayTargetRenderer::OnEntityDeleted(Entity entity) {
    if (std::holds_alternative<Entity>(displayTarget) && entity == std::get<Entity>(displayTarget)) {
        ResetDisplayTarget();
    }
}
void DisplayTargetRenderer::OnEntitySelected(Entity entity) {
    SetDisplayTarget(entity);
}
void DisplayTargetRenderer::OnEntityDeselected() {
    if (std::holds_alternative<Entity>(displayTarget)) {
        ResetDisplayTarget();
    }
}