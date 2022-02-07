#include "GUI.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ProjectSerializer.hpp>
#include <ProjectDeserializer.hpp>
#include <Window.hpp>
#include <Texture.hpp>

GUI::GUI(std::unique_ptr<Core>& core) noexcept :
	core(core),
	mb(this),
	sh(this),
	obs(this),
	con(this),
	am(this),
	sv(this),
	gv(this),
	delta(0) {
	ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
	SVGPathway::Initialize({ txtColor.x, txtColor.y, txtColor.z, txtColor.w });
	core->_window->SetTitle(defaultWindowName);
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
			ImGui::DockBuilderDockWindow(OBSERVER_ID, rightUp);
			ImGui::DockBuilderDockWindow(CONSOLE_ID, rightDown);
			ImGui::DockBuilderDockWindow(ASSET_MANAGER_ID, rightDown);
			ImGui::DockBuilderDockWindow(SCENE_VIEWPORT_ID, center);
			ImGui::DockBuilderDockWindow(GAME_VIEWPORT_ID, center);
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

	sh.Begin();
	if(HasOpenScene()) {
		sh.Render(openProject->_openScene.value());
	}
	sh.End();

	static std::optional<Scene> noScene;
	obs.Begin(HasOpenProject() ? openProject->_openScene : noScene);
	if (HasOpenScene()) {
		obs.Render(openProject->_openScene.value());
	}
	obs.End();

	con.Begin();
	con.Render();
	con.End();

	am.Begin();
	am.Render();
	am.End();

	sv.Begin();
	sv.Render();
	sv.End();

	gv.Begin();
	gv.Render();
	gv.End();

	End();
}

void GUI::End() {
	ImGui::End();
}

void GUI::CreateNewProject(std::string_view workspace, std::string_view name) {
	openProject.emplace(std::string(workspace), std::string(name));

	std::string title = defaultWindowName;
	title.append(" - ");
	title.append(openProject->_name);
	core->_window->SetTitle(title);
}

void GUI::SaveProjectToDisk() {
	if (openProject) {
		openProject->SaveToDisk();
	}
}

void GUI::OpenProjectFromDisk(const std::string& path) {
	FNode file;
	file._path = path;
	openProject.emplace(DeserializeProject(&file));

	std::string title = defaultWindowName;
	title.append(" - ");
	title.append(openProject->_name);
	core->_window->SetTitle(title);
}

void GUI::CloseProject() {
	openProject.reset();
}

void GUI::CreateNewScene(std::string_view relativePath, std::string_view name) {
	if (!HasOpenProject()) { return; }
	FNode* file = openProject->Assets().CreateNewSceneFileNode(relativePath, name);
	if (!HasOpenScene()) {
		openProject->OpenScene(file);
	}
}

bool GUI::HasOpenProject() { return openProject.has_value(); }
bool GUI::HasOpenScene() { return HasOpenProject() && openProject->_openScene.has_value(); }

ImGuiIO* GUI::IO() { return io; }
ImFont* GUI::GetFont() { return font; }
ImFont* GUI::GetFontBold() { return fontBold; }

void* GUI::GetFolderIcon(TextureSize size) { return reinterpret_cast<void*>(SVGPathway::Get(FOLDER_ICON_KEY, TextureStyle::PADDED, size).lock()->_glTextureID); }
void* GUI::GetProjectIcon(TextureSize size) { return reinterpret_cast<void*>(SVGPathway::Get(PROJECT_ICON_KEY, TextureStyle::PADDED, size).lock()->_glTextureID); }
void* GUI::GetSceneIcon(TextureSize size) { return reinterpret_cast<void*>(SVGPathway::Get(SCENE_ICON_KEY, TextureStyle::PADDED, size).lock()->_glTextureID); }
void* GUI::GetFileIcon(TextureSize size) { return reinterpret_cast<void*>(SVGPathway::Get(FILE_ICON_KEY, TextureStyle::PADDED, size).lock()->_glTextureID); }
void* GUI::GetBackArrowIcon(TextureSize size) { return reinterpret_cast<void*>(SVGPathway::Get(BACK_ARROW_ICON_KEY, TextureStyle::PADDED, size).lock()->_glTextureID); }
