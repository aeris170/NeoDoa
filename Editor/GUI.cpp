#include <Editor/GUI.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>

GUI::GUI(const CorePtr& core) noexcept :
    CORE(core),
    window(core->GetWindow()) {

    std::string title = defaultWindowName;
    title.append(" - [NO OPEN PROJECT]");
    window->SetTitle(title);

    Events.OnAssetOpened  += std::bind_front(&GUI::OnAssetOpened , this);
    Events.OnSceneOpened  += std::bind_front(&GUI::OnSceneOpened , this);
    Events.OnSceneClosed  += std::bind_front(&GUI::OnSceneClosed , this);
    Events.OnReimport     += std::bind_front(&GUI::OnReimport    , this);
    Events.OnAssetDeleted += std::bind_front(&GUI::OnAssetDeleted, this);

    //shortcutHandler.RegisterShortcut(Shortcuts::NewProjectShortcut,   [this]() { ShowNewProjectModal();  }, ImGuiInputFlags_RouteGlobal);
    //shortcutHandler.RegisterShortcut(Shortcuts::OpenProjectShortcut,  [this]() { ShowOpenProjectModal(); }, ImGuiInputFlags_RouteGlobal);
    shortcutHandler.RegisterShortcut(Shortcuts::SaveProjectShortcut,  [this]() { SaveProjectToDisk();    }, ImGuiInputFlags_RouteGlobal);
    shortcutHandler.RegisterShortcut(Shortcuts::CloseProjectShortcut, [this]() { CloseProject();         }, ImGuiInputFlags_RouteGlobal);

    shortcutHandler.RegisterShortcut(Shortcuts::UndoShortcut, [this]() { UndoLastCommand(); }, ImGuiInputFlags_RouteGlobal);
    shortcutHandler.RegisterShortcut(Shortcuts::RedoShortcut, [this]() { RedoLastCommand(); }, ImGuiInputFlags_RouteGlobal);

    shortcutHandler.RegisterShortcut(Shortcuts::NewSceneShortcut,   [this]() { ShowNewSceneAssetModal(*am.GetCurrentFolder()); }, ImGuiInputFlags_RouteGlobal);
    shortcutHandler.RegisterShortcut(Shortcuts::SaveSceneShortcut,  [this]() { SaveScene();  }, ImGuiInputFlags_RouteGlobal);
    shortcutHandler.RegisterShortcut(Shortcuts::CloseSceneShortcut, [this]() { CloseScene(); }, ImGuiInputFlags_RouteGlobal);
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

            ImGui::DockBuilderDockWindow(WindowStrings::SceneHierarchyWindowTitleID, left);

            ImGui::DockBuilderDockWindow(WindowStrings::SceneViewportWindowTitleID, center);
            ImGui::DockBuilderDockWindow(WindowStrings::GameViewportWindowTitleID, center);

            ImGui::DockBuilderDockWindow(WindowStrings::ObserverWindowTitleID, rightUp);
            ImGui::DockBuilderDockWindow(WindowStrings::SceneSettingsWindowTitleID, rightUp);
            ImGui::DockBuilderDockWindow(WindowStrings::CodeEditorWindowTitleID, rightUp);

            ImGui::DockBuilderDockWindow(WindowStrings::AssetManagerWindowTitleID, rightDown);
            ImGui::DockBuilderDockWindow(WindowStrings::ConsoleWindowTitleID, rightDown);

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

    if (urh.Begin()) {
        urh.Render();
    }
    urh.End();

    if (svcs.Begin(sv.GetViewportCameraSettingsButtonPosition())) {
        svcs.Render();
    }
    svcs.End();

    nam.Render();

    shortcutHandler.CheckShortcuts();

    End();
}

void GUI::End() {
    ImGui::End();
    ExecuteDockBuilderFocusWorkAround();

    if (ImGui::GetIO().WantSaveIniSettings) {
        ImGui::GetIO().WantSaveIniSettings = false;
        meta.SaveImGuiIniSettingsToDisk();
    }

    std::string_view message = request.Receive(ReceiveFlag::DontWait);
    if (message == "request_project_path") {
        reply.Send(projectPath);
    }
}

void GUI::CreateNewProject(std::string_view workspace, std::string_view name) {
    CORE->CreateAndLoadProject(workspace, name);
    Project& project = GetOpenProject();
    AssetHandle handle = CORE->GetAssets()->CreateAssetAt<Scene>(CORE->GetAssets()->Root(), "Sample Scene.scn", std::string("Sample Scene"));
    assert(handle.HasValue());
    project.SetStartupScene(handle->ID());
    OpenScene(handle);

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(project.Name());
    window->SetTitle(title);

    Events.OnProjectLoaded(project);
}

void GUI::SaveProjectToDisk() {
    if (HasOpenScene()) {
        SaveScene();
    }
    if (HasOpenProject()) {
        CORE->SaveLoadedProjectToDisk();
        Events.OnProjectSaved(GetOpenProject());
    }
}

void GUI::OpenProjectFromDisk(const std::string& path) {
    CORE->LoadProject(path);
    assert(HasOpenProject());

    Project& project = GetOpenProject();

    std::string title = defaultWindowName;
    title.append(" - [");
    title.append(project.Name());
    title.append("]");
    window->SetTitle(title);

    /* CORE->LoadProject may load start-up scene (if it exists), we need to trap open scene */
    if (project.HasOpenScene()) {
        Events.OnSceneOpened(project.GetOpenScene());
        sceneUUID = project.GetOpenSceneID();
    }

    projectPath = path;

    Events.OnProjectLoaded(project);
}

void GUI::CloseProject() {
    Events.OnProjectUnloaded();

    CloseScene();
    CORE->UnloadProject();

    std::string title = defaultWindowName;
    title.append(" - [NO OPEN PROJECT]");
    window->SetTitle(title);
}

void GUI::CreateNewScene(FNode& folder, std::string_view name) {
    if (!HasOpenProject()) { return; }

    const Scene temporary(name); const auto data = temporary.Serialize();
    AssetHandle handle = CORE->GetAssets()->CreateAssetAt<Scene>(folder, std::string(name) + Assets::SceneExtension, data);
    assert(handle.HasValue());
    DOA_LOG_INFO("Succesfully created a new scene asset named %s at %s", name.data(), folder.Path().c_str());
    Events.OnAssetCreated(handle);
    if (!HasOpenScene()) {
        OpenScene(handle);
    }
}
void GUI::OpenScene(AssetHandle sceneHandle) {
    assert(HasOpenProject());
    assert(sceneHandle.HasValue());
    assert(sceneHandle->IsScene());

    GetOpenProject().OpenScene(sceneHandle->ID());
    Events.OnAssetOpened(sceneHandle);
    Events.OnSceneOpened(GetOpenProject().GetOpenScene());

    history.Clear();
}
void GUI::SaveScene() const {
    if (sceneUUID == UUID::Empty()) { return; }

    AssetHandle handle = CORE->GetAssets()->FindAsset(sceneUUID);
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

    history.Clear();
}

bool GUI::HasOpenProject() const { return CORE->HasLoadedProject(); }
Project& GUI::GetOpenProject() const { return *CORE->LoadedProject().get(); }
bool GUI::HasOpenScene() const { return sceneUUID != UUID::Empty(); }
Scene& GUI::GetOpenScene() { return scene.value(); }

MenuBar& GUI::GetMenuBar()                                                     { return mb;   }
SceneHierarchy& GUI::GetSceneHierarchy()                                       { return sh;   }
Observer& GUI::GetObserver()                                                   { return obs;  }
CodeEditor& GUI::GetCodeEditor()                                               { return ce;   }
AssetManager& GUI::GetAssetManager()                                           { return am;   }
Console& GUI::GetConsole()                                                     { return con;  }
SceneViewport& GUI::GetSceneViewport()                                         { return sv;   }
GameViewport& GUI::GetGameViewport()                                           { return gv;   }
SceneSettings& GUI::GetSceneSettings()                                         { return ss;   }
UndoRedoHistory& GUI::GetUndoRedoHistory()                                     { return urh;  }
SceneViewportCameraSettings& GUI::GetSceneViewportCameraSettings()             { return svcs; }
const MenuBar& GUI::GetMenuBar() const                                         { return mb;   }
const SceneHierarchy& GUI::GetSceneHierarchy() const                           { return sh;   }
const Observer& GUI::GetObserver() const                                       { return obs;  }
const CodeEditor& GUI::GetCodeEditor() const                                   { return ce;   }
const AssetManager& GUI::GetAssetManager() const                               { return am;   }
const Console& GUI::GetConsole() const                                         { return con;  }
const SceneViewport& GUI::GetSceneViewport() const                             { return sv;   }
const GameViewport& GUI::GetGameViewport() const                               { return gv;   }
const SceneSettings& GUI::GetSceneSettings() const                             { return ss;   }
const UndoRedoHistory& GUI::GetUndoRedoHistory() const                         { return urh;  }
const SceneViewportCameraSettings& GUI::GetSceneViewportCameraSettings() const { return svcs; }

ImGuiIO* GUI::IO() const { return io; }
ImFont* GUI::GetFont() const { return font; }
ImFont* GUI::GetFontBold() const { return fontBold; }

void* GUI::GetFolderIcon(TextureSize size) const                       { return SVGPathway::Get(FOLDER_ICON_KEY,           TextureStyle::PADDED, size); }
void* GUI::GetProjectIcon(TextureSize size) const                      { return SVGPathway::Get(PROJECT_ICON_KEY,          TextureStyle::PADDED, size); }
void* GUI::GetSceneIcon(TextureSize size) const                        { return SVGPathway::Get(SCENE_ICON_KEY,            TextureStyle::PADDED, size); }
void* GUI::GetComponentIcon(TextureSize size) const                    { return SVGPathway::Get(COMPONENT_ICON_KEY,        TextureStyle::PADDED, size); }
void* GUI::GetSamplerIcon(TextureSize size) const                      { return SVGPathway::Get(SAMPLER_ICON_KEY,          TextureStyle::PADDED, size); }
void* GUI::GetTextureIcon(TextureSize size) const                      { return SVGPathway::Get(TEXTURE_ICON_KEY,          TextureStyle::PADDED, size); }
void* GUI::GetVertexShaderIcon(TextureSize size) const                 { return SVGPathway::Get(VERTEX_SHADER_ICON_KEY,    TextureStyle::PADDED, size); }
void* GUI::GetTessellationControlShaderIcon(TextureSize size) const    { return SVGPathway::Get(TESS_CTRL_SHADER_ICON_KEY, TextureStyle::PADDED, size); }
void* GUI::GetTessellationEvaluationShaderIcon(TextureSize size) const { return SVGPathway::Get(TESS_EVAL_SHADER_ICON_KEY, TextureStyle::PADDED, size); }
void* GUI::GetGeometryShaderIcon(TextureSize size) const               { return SVGPathway::Get(GEOMETRY_SHADER_ICON_KEY,  TextureStyle::PADDED, size); }
void* GUI::GetFragmentShaderIcon(TextureSize size) const               { return SVGPathway::Get(FRAGMENT_SHADER_ICON_KEY,  TextureStyle::PADDED, size); }
void* GUI::GetComputeShaderIcon(TextureSize size) const                { return SVGPathway::Get(COMPUTE_SHADER_ICON_KEY,   TextureStyle::PADDED, size); }
void* GUI::GetMaterialIcon(TextureSize size) const                     { return SVGPathway::Get(MATERIAL_ICON_KEY,         TextureStyle::PADDED, size); }
void* GUI::GetFrameBufferIcon(TextureSize size) const                  { return SVGPathway::Get(FRAMEBUFFER_ICON_KEY,      TextureStyle::PADDED, size); }
void* GUI::GetFileIcon(TextureSize size) const                         { return SVGPathway::Get(FILE_ICON_KEY,             TextureStyle::PADDED, size); }
void* GUI::GetBackArrowIcon(TextureSize size) const                    { return SVGPathway::Get(BACK_ARROW_ICON_KEY,       TextureStyle::PADDED, size); }

void* GUI::FindIconForFileType(const FNode& file, TextureSize size) const {
    assert(HasOpenProject());

    if (file.IsDirectory()) { return GetFolderIcon(size); }
    if (Assets::IsProjectFile(file)) { return GetProjectIcon(size); }

    assert(CORE->GetAssets()->IsAssetExistsAt(file));
    AssetHandle asset = CORE->GetAssets()->FindAssetAt(file);

    if (asset->IsScene())                                              { return GetSceneIcon(size);                        }
    if (asset->IsComponentDefinition())                                { return GetComponentIcon(size);                    }
    if (asset->IsSampler())                                            { return GetSamplerIcon(size);                      }
    if (asset->IsTexture())                                            { return GetTextureIcon(size);                      }
    if (asset->IsShader()) {
        if (Assets::IsVertexShaderFile(asset->File()))                 { return GetVertexShaderIcon(size);                 }
        if (Assets::IsTessellationControlShaderFile(asset->File()))    { return GetTessellationControlShaderIcon(size);    }
        if (Assets::IsTessellationEvaluationShaderFile(asset->File())) { return GetTessellationEvaluationShaderIcon(size); }
        if (Assets::IsGeometryShaderFile(asset->File()))               { return GetGeometryShaderIcon(size);               }
        if (Assets::IsFragmentShaderFile(asset->File()))               { return GetFragmentShaderIcon(size);               }
        if (Assets::IsComputeShaderFile(asset->File()))                { return GetComputeShaderIcon(size);                }
    }
    if (asset->IsMaterial())                                           { return GetMaterialIcon(size);                     }
    if (asset->IsFrameBuffer())                                        { return GetFrameBufferIcon(size);                  }
    if (asset->IsModel())                                              { return GetSceneIcon(size);                        }
    return GetFileIcon(size);
}
void* GUI::FindIconByName(const std::string_view key, TextureSize size) const { return reinterpret_cast<void*>(static_cast<uint64_t>(SVGPathway::Get(std::string(key), TextureStyle::PADDED, size).GLObjectID)); }

MetaAssetInfo& GUI::GetMetaInfoOf(const FNode& file) { return meta.GetMetaAssetInfoBank().GetMetaInfoOf(file); }
MetaAssetInfoBank& GUI::GetMetaAssetInfoBank() noexcept { return meta.GetMetaAssetInfoBank(); }

void GUI::ShowNewSceneAssetModal(FNode& currentFolder) const                        { nam.ShowSceneCreationModal(currentFolder);                        }
void GUI::ShowNewComponentAssetModal(FNode& currentFolder) const                    { nam.ShowComponentCreationModal(currentFolder);                    }
void GUI::ShowNewSamplerAssetModal(FNode& currentFolder) const                      { nam.ShowSamplerCreationModal(currentFolder);                      }
void GUI::ShowNewVertexShaderAssetModal(FNode& currentFolder) const                 { nam.ShowVertexShaderCreationModal(currentFolder);                 }
void GUI::ShowNewTessellationControlShaderAssetModal(FNode& currentFolder) const    { nam.ShowTessellationControlShaderCreationModal(currentFolder);    }
void GUI::ShowNewTessellationEvaluationShaderAssetModal(FNode& currentFolder) const { nam.ShowTessellationEvaluationShaderCreationModal(currentFolder); }
void GUI::ShowNewGeometryShaderAssetModal(FNode& currentFolder) const               { nam.ShowGeometryShaderCreationModal(currentFolder);               }
void GUI::ShowNewFragmentShaderAssetModal(FNode& currentFolder) const               { nam.ShowFragmentShaderCreationModal(currentFolder);               }
void GUI::ShowNewShaderProgramAssetModal(FNode& currentFolder) const                { nam.ShowShaderProgramCreationModal(currentFolder);                }
void GUI::ShowNewMaterialAssetModal(FNode& currentFolder) const                     { nam.ShowMaterialCreationModal(currentFolder);                     }
void GUI::ShowNewFrameBufferAssetModal(FNode& currentFolder) const                  { nam.ShowFrameBufferCreationModal(currentFolder);                  }

UndoRedoStack& GUI::GetCommandHistory() noexcept { return history; }
void GUI::UndoLastCommand() noexcept {
    if (!history.CanUndo()) { return; }
    history.Undo();
}
void GUI::RedoLastCommand() noexcept {
    if (!history.CanRedo()) { return; }
    history.Redo();
}
bool GUI::CanUndoLastCommand() const noexcept { return history.CanUndo(); }
bool GUI::CanRedoLastCommand() const noexcept { return history.CanRedo(); }

// TODO REMOVE ME WHEN IMGUI IMPLEMENTS THIS WORKAROUND AS API FUNC.
void GUI::ExecuteDockBuilderFocusWorkAround() {
    static int i = -1;
    if (i == 0) {
        auto asset = ImGui::FindWindowByName(WindowStrings::AssetManagerWindowTitleID);
        ImGui::FocusWindow(asset);

        auto obs = ImGui::FindWindowByName(WindowStrings::ObserverWindowTitleID);
        ImGui::FocusWindow(obs);

        auto viewport = ImGui::FindWindowByName(WindowStrings::SceneViewportWindowTitleID);
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
    meta.GetMetaAssetInfoBank().Clear();
    if (sceneUUID != UUID::Empty()) {
        AssetHandle currentSceneHandle = assets.FindAsset(sceneUUID);
        if (!currentSceneHandle.HasValue()) {
            sceneUUID = UUID::Empty();
            scene = std::nullopt;
        }
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