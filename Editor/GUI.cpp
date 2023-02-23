#include "GUI.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ProjectSerializer.hpp>
#include <ProjectDeserializer.hpp>
#include <Window.hpp>
#include <Texture.hpp>

#include "Icons.hpp"

GUI::GUI(const CorePtr& core) noexcept :
    CORE(core),
    window(core->Window()) {
    FileDialog::Initialize();
    window->SetTitle(defaultWindowName);
}

void GUI::Prepare() {
    window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    dockspace_flags = ImGuiDockNodeFlags_NoCloseButton;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin(dockSpaceName, &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    io = &ImGui::GetIO();
    font = io->Fonts->Fonts[0];
    fontBold = io->Fonts->Fonts[1];

    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize = { 370, 150 };
    style.WindowMenuButtonPosition = ImGuiDir_None;
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("NeoDoa Dockspace");

        if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
            // Clear out existing layout
            ImGui::DockBuilderRemoveNode(dockspace_id);
            // Add empty node
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            // Main node should cover entire window
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            // Build dock layout
            ImGuiID center = dockspace_id;
            ImGuiID left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.25f, nullptr, &center);
            ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &center);

            ImGuiID rightDown;
            ImGuiID rightUp = ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.65f, nullptr, &rightDown);

            ImGui::DockBuilderDockWindow(SCENE_HIERARCHY_ID, left);

            ImGui::DockBuilderDockWindow(SCENE_VIEWPORT_ID, center);
            ImGui::DockBuilderDockWindow(GAME_VIEWPORT_ID, center);

            ImGui::DockBuilderDockWindow(OBSERVER_ID, rightUp);
            ImGui::DockBuilderDockWindow(SCENE_SETTINGS_ID, rightUp);
            ImGui::DockBuilderDockWindow(CODE_EDITOR_ID, rightUp);

            ImGui::DockBuilderDockWindow(ASSET_MANAGER_ID, rightDown);
            ImGui::DockBuilderDockWindow(CONSOLE_ID, rightDown);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpace(dockspace_id, { 0, 0 }, dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void GUI::operator() (float delta) {
    ImGui::ShowDemoWindow();
    this->delta = delta;
    Prepare();

    if (mb.Begin()) {
        mb.Render();
    }
    mb.End();

    if (am.Begin()) {
        am.Render();
    }
    am.End();

    if (sh.Begin()) {
        if (HasOpenScene()) {
            sh.Render(GetOpenScene());
        }
    }
    sh.End();

    Scene* scenePtr = HasOpenProject() && HasOpenScene() ? &GetOpenScene() : nullptr;
    if (obs.Begin(scenePtr)) {
        if (HasOpenScene()) {
            obs.Render(GetOpenScene());
        }
    }
    obs.End();

    if (ce.Begin()) {
        ce.Render();
    }
    ce.End();

    if (con.Begin()) {
        con.Render();
    }
    con.End();

    if (sv.Begin(scenePtr)) {
        if (HasOpenScene()) {
            sv.Render(GetOpenScene());
        }
    }
    sv.End();

    if (gv.Begin()) {
        gv.Render();
    }
    gv.End();

    if (ss.Begin(scenePtr)) {
        if (HasOpenScene()) {
            ss.Render(GetOpenScene());
        }
    }
    ss.End();

    End();
}

void GUI::End() {
    ImGui::End();
    ExecuteDockBuilderFocusWorkAround();
}

void GUI::CreateNewProject(std::string_view workspace, std::string_view name) {
    CORE->CreateAndLoadProject(workspace, name);
    metaInfo.Clear();
    metaInfo.SaveToDisk(CORE->LoadedProject()->Workspace());

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(CORE->LoadedProject()->Name());
    window->SetTitle(title);
}

void GUI::SaveProjectToDisk() {
    if (CORE->HasLoadedProject()) {
        CORE->SaveLoadedProjectToDisk();
        metaInfo.SaveToDisk(CORE->LoadedProject()->Workspace());
    }
}

void GUI::OpenProjectFromDisk(const std::string& path) {
    CORE->LoadProject(path);
    metaInfo.LoadFromDisk(*CORE->LoadedProject());

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(CORE->LoadedProject()->Name());
    window->SetTitle(title);
}

void GUI::CloseProject() {
    obs.ResetDisplayTarget();
    CORE->UnloadProject();
}

void GUI::CreateNewScene(std::string_view relativePath, std::string_view name) {
    // TODO
    /*
    if (!HasOpenProject()) { return; }
    FNode* file = openProject->Assets().CreateNewSceneFileNode(relativePath, name);
    if (!HasOpenScene()) {
        openProject->OpenScene(file);
    }
    */
}

bool GUI::HasOpenProject() const { return CORE->HasLoadedProject(); }
Project& GUI::GetOpenProject() const { return *CORE->LoadedProject().get(); }
bool GUI::HasOpenScene() const { return HasOpenProject() && CORE->LoadedProject()->HasOpenScene(); }
Scene& GUI::GetOpenScene() const { return CORE->LoadedProject()->GetOpenScene(); }

ImGuiIO* GUI::IO() const { return io; }
ImFont* GUI::GetFont() const { return font; }
ImFont* GUI::GetFontBold() const { return fontBold; }

void* GUI::GetFolderIcon(TextureSize size) const    { return SVGPathway::Get(FOLDER_ICON_KEY,     TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetProjectIcon(TextureSize size) const   { return SVGPathway::Get(PROJECT_ICON_KEY,    TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetSceneIcon(TextureSize size) const     { return SVGPathway::Get(SCENE_ICON_KEY,      TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetComponentIcon(TextureSize size) const { return SVGPathway::Get(COMPONENT_ICON_KEY,  TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetFileIcon(TextureSize size) const      { return SVGPathway::Get(FILE_ICON_KEY,       TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetBackArrowIcon(TextureSize size) const { return SVGPathway::Get(BACK_ARROW_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }

void* GUI::FindIconForFileType(const FNode& file, TextureSize size) const {
    assert(HasOpenProject());

    if (file.IsDirectory()) { return GetFolderIcon(size); }
    if (file.Extension() == ".doa") { return GetProjectIcon(size); } /* TODO FIX THIS SHITTY EXTENSION CHECK */

    assert(CORE->Assets()->IsAssetExistsAt(file));
    AssetHandle asset = CORE->Assets()->FindAssetAt(file);

    if (asset->IsScene()) { return GetSceneIcon(size); }
    if (asset->IsComponentDefinition()) { return GetComponentIcon(size); }
    if (asset->IsScript()) { return GetSceneIcon(size); }
    if (asset->IsTexture()) { return GetSceneIcon(size); }
    if (asset->IsModel()) { return GetSceneIcon(size); }
    if (asset->IsMaterial()) { return GetSceneIcon(size); }
    if (asset->IsShader()) { return GetSceneIcon(size); }
    return GetFileIcon(size);
}
void* GUI::FindIconByName(const std::string_view key, TextureSize size) const { return SVGPathway::Get(std::string(key), TextureStyle::PADDED, size).TextureIDRaw(); }

MetaAssetInfo& GUI::GetMetaInfoOf(const FNode& file) { return metaInfo.GetMetaInfoOf(file); }
void GUI::ReloadMetaInfo() { return metaInfo.LoadFromDisk(*CORE->LoadedProject()); }

// TODO REMOVE ME WHEN IMGUI IMPLEMENTS THIS WORKAROUND AS API FUNC.
void GUI::ExecuteDockBuilderFocusWorkAround() {
    static int i = -1;
    if (i == 0) {
        auto asset = ImGui::FindWindowByName(ASSET_MANAGER_ID);
        ImGui::FocusWindow(asset);

        auto obs = ImGui::FindWindowByName(OBSERVER_ID);
        ImGui::FocusWindow(obs);

        auto viewport = ImGui::FindWindowByName(SCENE_VIEWPORT_ID);
        ImGui::FocusWindow(viewport);
    }
    i++;
}
