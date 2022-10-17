#include "GUI.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ProjectSerializer.hpp>
#include <ProjectDeserializer.hpp>
#include <Window.hpp>
#include <Texture.hpp>

GUI::GUI(const CorePtr& core) noexcept :
    core(core),
    window(core->Window()),
    mb(*this),
    sh(*this),
    obs(*this),
    am(*this),
    con(*this),
    sv(*this),
    gv(*this),
    ss(*this),
    delta(0) {
    ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    SVGPathway::Initialize({ txtColor.x, txtColor.y, txtColor.z, txtColor.w });
    window->SetTitle(defaultWindowName);
}

void GUI::Prepare() {
    window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
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

    mb.Begin();
    mb.Render();
    mb.End();

    am.Begin();
    am.Render();
    am.End();

    sh.Begin();
    if (HasOpenScene()) {
        sh.Render(openProject->GetOpenScene());
    }
    sh.End();

    obs.Begin(HasOpenProject() ? &openProject->GetOpenScene() : nullptr);
    if (HasOpenScene()) {
        obs.Render(openProject->GetOpenScene());
    }
    obs.End();

    con.Begin();
    con.Render();
    con.End();

    sv.Begin(HasOpenProject() ? &openProject->GetOpenScene() : nullptr);
    if (HasOpenScene()) {
        sv.Render(openProject->GetOpenScene());
    }
    sv.End();

    gv.Begin();
    gv.Render();
    gv.End();

    ss.Begin(HasOpenProject() ? &openProject->GetOpenScene() : nullptr);
    if (HasOpenScene()) {
        ss.Render(openProject->GetOpenScene());
    }
    ss.End();

    End();
}

void GUI::End() {
    ImGui::End();
    ExecuteDockBuilderFocusWorkAround();
}

void GUI::CreateNewProject(std::string_view workspace, std::string_view name) {
    openProject.emplace(std::string(workspace), std::string(name));
    core->LoadProject(openProject.value());

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(openProject->Name());
    window->SetTitle(title);
}

void GUI::SaveProjectToDisk() {
    if (openProject) {
        openProject->SaveToDisk();
    }
}

void GUI::OpenProjectFromDisk(const std::string& path) {
    CloseProject();

    // TODO
    FNode file(FNodeCreationParams{
        nullptr,
        nullptr,
        path
    });
    openProject.emplace(DeserializeProject(&file));
    core->LoadProject(openProject.value());

    std::string title = defaultWindowName;
    title.append(" - ");
    title.append(openProject->Name());
    window->SetTitle(title);
}

void GUI::CloseProject() {
    obs.ResetDisplayTarget();
    openProject.reset();
    core->UnloadProject();
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

bool GUI::HasOpenProject() { return openProject.has_value(); }
bool GUI::HasOpenScene() { return HasOpenProject() && openProject->HasOpenScene(); }

ImGuiIO* GUI::IO() { return io; }
ImFont* GUI::GetFont() { return font; }
ImFont* GUI::GetFontBold() { return fontBold; }

void* GUI::GetFolderIcon(TextureSize size) { return SVGPathway::Get(FOLDER_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetProjectIcon(TextureSize size) { return SVGPathway::Get(PROJECT_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetSceneIcon(TextureSize size) { return SVGPathway::Get(SCENE_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetFileIcon(TextureSize size) { return SVGPathway::Get(FILE_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }
void* GUI::GetBackArrowIcon(TextureSize size) { return SVGPathway::Get(BACK_ARROW_ICON_KEY, TextureStyle::PADDED, size).TextureIDRaw(); }

void* GUI::FindIconForFileType(const FNode& file, TextureSize size) {
    assert(HasOpenProject());

    if (file.IsDirectory()) { return GetFolderIcon(size); }
    if (file.Extension() == ".doa") { return GetProjectIcon(size); } /* TODO FIX THIS SHITTY EXTENSION CHECK */

    Project& project = openProject.value();
    assert(project.Assets().IsAssetExistsAt(file));
    AssetHandle asset = project.Assets().FindAssetAt(file);

    if (asset->IsScene()) { return GetSceneIcon(size); }
    if (asset->IsScript()) { return GetSceneIcon(size); }
    if (asset->IsTexture()) { return GetSceneIcon(size); }
    if (asset->IsModel()) { return GetSceneIcon(size); }
    if (asset->IsMaterial()) { return GetSceneIcon(size); }
    if (asset->IsShader()) { return GetSceneIcon(size); }
    return GetFileIcon(size);
}

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
