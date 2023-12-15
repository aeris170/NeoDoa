#include <Editor/GUI.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <Editor/Icons.hpp>

GUI::GUI(const CorePtr& core) noexcept :
    CORE(core),
    window(core->Window()) {
    FileDialog::Initialize();

    std::string title = defaultWindowName;
    title.append(" - [NO OPEN PROJECT]");
    window->SetTitle(title);

    Events.OnAssetOpened  += std::bind_front(&GUI::OnAssetOpened , this);
    Events.OnSceneOpened  += std::bind_front(&GUI::OnSceneOpened , this);
    Events.OnSceneClosed  += std::bind_front(&GUI::OnSceneClosed , this);
    Events.OnReimport     += std::bind_front(&GUI::OnReimport    , this);
    Events.OnAssetDeleted += std::bind_front(&GUI::OnAssetDeleted, this);
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
        sh.Render();
    }
    sh.End();

    if (obs.Begin()) {
        obs.Render();
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

    if (sv.Begin()) {
        sv.Render();
    }
    sv.End();

    if (gv.Begin()) {
        gv.Render();
    }
    gv.End();

    if (ss.Begin()) {
        ss.Render();
    }
    ss.End();

    npm.Render();

    End();
}

void GUI::End() {
    ImGui::End();
    ExecuteDockBuilderFocusWorkAround();
}

void GUI::CreateNewProject(std::string_view workspace, std::string_view name) {
    CORE->CreateAndLoadProject(workspace, name);
    Project& project = GetOpenProject();
    metaInfo.Clear();
    metaInfo.SaveToDisk(project.Workspace());
    AssetHandle handle = CORE->Assets()->CreateAssetAt<Scene>(CORE->Assets()->Root(), "Sample Scene.scn", std::string("Sample Scene"));
    assert(handle.HasValue());
    project.SetStartupScene(handle->ID());
    OpenScene(handle);

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(project.Name());
    window->SetTitle(title);
}

void GUI::SaveProjectToDisk() {
    if (HasOpenProject()) {
        CORE->SaveLoadedProjectToDisk();
        metaInfo.SaveToDisk(GetOpenProject().Workspace());
    }
}

void GUI::OpenProjectFromDisk(const std::string& path) {
    CORE->LoadProject(path);
    assert(HasOpenProject());

    Project& project = GetOpenProject();
    metaInfo.LoadFromDisk(project, *CORE->Assets());

    std::string title = defaultWindowName;
    title.append(" - [");
    title.append(project.Name());
    title.append("]");
    window->SetTitle(title);

    /* CORE->LoadProject may load start-up scene (if it exists), we need to trap open scene */
    if (HasOpenScene()) {
        Events.OnSceneOpened(project.GetOpenScene());
        sceneUUID = project.GetOpenSceneID();
    }
}

void GUI::CloseProject() {
    obs.ResetDisplayTarget();
    CORE->UnloadProject();

    std::string title = defaultWindowName;
    title.append(" - [NO OPEN PROJECT]");
    window->SetTitle(title);
}

void GUI::CreateNewScene(std::string_view relativePath, std::string_view name) {
    if (!HasOpenProject()) { return; }
    // TODO
    /*
    FNode* file = openProject->Assets().CreateNewSceneFileNode(relativePath, name);
    if (!HasOpenScene()) {
        openProject->OpenScene(file);
    }
    */
}
void GUI::OpenScene(AssetHandle sceneHandle) {
    assert(HasOpenProject());
    assert(sceneHandle.HasValue());
    assert(sceneHandle->IsScene());

    GetOpenProject().OpenScene(sceneHandle->ID());
    Events.OnAssetOpened(sceneHandle);
    Events.OnSceneOpened(GetOpenProject().GetOpenScene());
}
void GUI::SaveScene() const {
    if (sceneUUID == UUID::Empty()) { return; }

    AssetHandle handle = CORE->Assets()->FindAsset(sceneUUID);
    if (handle.HasValue()) {
        assert(handle->IsScene());
        Scene& sceneData = handle->DataAs<Scene>();
        sceneData = Scene::Copy(scene.value());
        handle->Serialize();
    } else {
        // TODO
        // Original asset is deleted while the scene was open, prompt user to save it to a new file.
    }
}
void GUI::CloseScene() {
    Events.OnSceneClosed();
}

bool GUI::HasOpenProject() const { return CORE->HasLoadedProject(); }
Project& GUI::GetOpenProject() const { return *CORE->LoadedProject().get(); }
bool GUI::HasOpenScene() const { return sceneUUID != UUID::Empty(); }
Scene& GUI::GetOpenScene() { return scene.value(); }

ImGuiIO* GUI::IO() const { return io; }
ImFont* GUI::GetFont() const { return font; }
ImFont* GUI::GetFontBold() const { return fontBold; }

void* GUI::GetFolderIcon(TextureSize size) const                       { return SVGPathway::Get(FOLDER_ICON_KEY,           TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetProjectIcon(TextureSize size) const                      { return SVGPathway::Get(PROJECT_ICON_KEY,          TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetSceneIcon(TextureSize size) const                        { return SVGPathway::Get(SCENE_ICON_KEY,            TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetComponentIcon(TextureSize size) const                    { return SVGPathway::Get(COMPONENT_ICON_KEY,        TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetVertexShaderIcon(TextureSize size) const                 { return SVGPathway::Get(VERTEX_SHADER_ICON_KEY,    TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetTessellationControlShaderIcon(TextureSize size) const    { return SVGPathway::Get(TESS_CTRL_SHADER_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetTessellationEvaluationShaderIcon(TextureSize size) const { return SVGPathway::Get(TESS_EVAL_SHADER_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetGeometryShaderIcon(TextureSize size) const               { return SVGPathway::Get(GEOMETRY_SHADER_ICON_KEY,  TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetFragmentShaderIcon(TextureSize size) const               { return SVGPathway::Get(FRAGMENT_SHADER_ICON_KEY,  TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetComputeShaderIcon(TextureSize size) const                { return SVGPathway::Get(COMPUTE_SHADER_ICON_KEY,   TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetFileIcon(TextureSize size) const                         { return SVGPathway::Get(FILE_ICON_KEY,             TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetBackArrowIcon(TextureSize size) const                    { return SVGPathway::Get(BACK_ARROW_ICON_KEY,       TextureStyle::PADDED, size).TextureIDRaw(); }

void* GUI::FindIconForFileType(const FNode& file, TextureSize size) const {
    assert(HasOpenProject());

    if (file.IsDirectory()) { return GetFolderIcon(size); }
    if (file.Extension() == ".doa") { return GetProjectIcon(size); } /* TODO FIX THIS SHITTY EXTENSION CHECK */

    assert(CORE->Assets()->IsAssetExistsAt(file));
    AssetHandle asset = CORE->Assets()->FindAssetAt(file);

    if (asset->IsScene())                                              { return GetSceneIcon(size);                        }
    if (asset->IsComponentDefinition())                                { return GetComponentIcon(size);                    }
    if (asset->IsScript())                                             { return GetSceneIcon(size);                        }
    if (asset->IsTexture())                                            { return GetSceneIcon(size);                        }
    if (asset->IsModel())                                              { return GetSceneIcon(size);                        }
    if (asset->IsMaterial())                                           { return GetSceneIcon(size);                        }
    if (asset->IsShader()) {
        if (Assets::IsVertexShaderFile(asset->File()))                 { return GetVertexShaderIcon(size);                 }
        if (Assets::IsTessellationControlShaderFile(asset->File()))    { return GetTessellationControlShaderIcon(size);    }
        if (Assets::IsTessellationEvaluationShaderFile(asset->File())) { return GetTessellationEvaluationShaderIcon(size); }
        if (Assets::IsGeometryShaderFile(asset->File()))               { return GetGeometryShaderIcon(size);               }
        if (Assets::IsFragmentShaderFile(asset->File()))               { return GetFragmentShaderIcon(size);               }
        if (Assets::IsComputeShaderFile(asset->File()))                { return GetComputeShaderIcon(size);                }
    }
    return GetFileIcon(size);
}
void* GUI::FindIconByName(const std::string_view key, TextureSize size) const { return SVGPathway::Get(std::string(key), TextureStyle::PADDED, size).TextureIDRaw(); }

MetaAssetInfo& GUI::GetMetaInfoOf(const FNode& file) { return metaInfo.GetMetaInfoOf(file); }
void GUI::ReloadMetaInfo() { metaInfo.LoadFromDisk(*CORE->LoadedProject(), *CORE->Assets()); }

void GUI::ShowNewProjectModal() const { npm.Show(); }

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

// -- //
// Open Scene
void GUI::OnAssetOpened(AssetHandle asset) {
    assert(asset.HasValue());
    if (asset->IsScene()) {
        sceneUUID = asset->ID();
    }
}
void GUI::OnSceneOpened(Scene& scene) {
    this->scene = Scene::Copy(scene);
}
void GUI::OnSceneClosed() {
    sceneUUID = UUID::Empty();
    scene = std::nullopt;
}
void GUI::OnReimport(Assets& assets) {
    if (sceneUUID == UUID::Empty()) { return; }

    AssetHandle currentSceneHandle = assets.FindAsset(sceneUUID);
    if (!currentSceneHandle.HasValue()) {
        sceneUUID = UUID::Empty();
        scene = std::nullopt;
    }
}
void GUI::OnAssetDeleted(AssetHandle asset) {
    assert(asset.HasValue());
    if (asset->ID() == sceneUUID) {
        sceneUUID = {};
        scene = std::nullopt;
    }
}
// -- //