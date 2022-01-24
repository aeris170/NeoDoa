#include "GUI.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ProjectSerializer.hpp>
#include <ProjectDeserializer.hpp>
#include <Window.hpp>
#include <Texture.hpp>

#include "SVGPathway.hpp"

const std::unordered_map<std::string, const std::string> GUI::CUSTOM_SCRIPT_ICONS = {
	{ "IDComponent", ICON_FA_ADDRESS_CARD " " },
	{ "TransformComponent", ICON_FA_SEEDLING " " },
	{ "ParentComponent", ICON_FA_USER_CROWN " " },
	{ "ChildComponent", ICON_FA_USER " " },
	{ "Camera", ICON_FA_VIDEO " " },
	{ "ModelRenderer", ICON_FA_COCKTAIL " " }
};

GUI::GUI(std::unique_ptr<Core>& core) noexcept :
	core(core),
	mb(this),
	sh(this),
	obs(this),
	con(this),
	am(this),
	delta(0) {
	ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
	SVGPathway::Initialize({ txtColor.x, txtColor.y, txtColor.z, txtColor.w });
	core->_window->SetTitle(defaultWindowName);
}

void GUI::Prepare() {
	window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	dockspace_flags = ImGuiDockNodeFlags_NoCloseButton;
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen) ImGui::PopStyleVar(2);

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
			ImGuiID left;
			ImGuiID right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.5f, nullptr, &left);

			ImGuiID leftLeft;
			ImGuiID leftRight = ImGui::DockBuilderSplitNode(left, ImGuiDir_Right, 0.5f, nullptr, &leftLeft);

			ImGuiID rightLeft;
			ImGuiID rightRight = ImGui::DockBuilderSplitNode(right, ImGuiDir_Right, 0.5f, nullptr, &rightLeft);

			ImGuiID rightRightDown;
			ImGuiID rightRightUp = ImGui::DockBuilderSplitNode(rightRight, ImGuiDir_Up, 0.6f, nullptr, &rightRightDown);

			ImGui::DockBuilderDockWindow(SCENE_HIERARCHY_ID, leftLeft);
			ImGui::DockBuilderDockWindow(OBSERVER_ID, rightRightUp);
			ImGui::DockBuilderDockWindow(ASSET_MANAGER_ID, rightRightDown);
			ImGui::DockBuilderDockWindow(CONSOLE_ID, rightRightDown);
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

void* GUI::GetFolderIcon() { return reinterpret_cast<void*>(SVGPathway::Get(FOLDER_ICON_KEY, TextureStyle::PADDED).lock()->_glTextureID); }
void* GUI::GetProjectIcon() { return reinterpret_cast<void*>(SVGPathway::Get(PROJECT_ICON_KEY, TextureStyle::PADDED).lock()->_glTextureID); }
void* GUI::GetSceneIcon() { return reinterpret_cast<void*>(SVGPathway::Get(SCENE_ICON_KEY, TextureStyle::PADDED).lock()->_glTextureID); }
void* GUI::GetFileIcon() { return reinterpret_cast<void*>(SVGPathway::Get(FILE_ICON_KEY, TextureStyle::PADDED).lock()->_glTextureID); }
void* GUI::GetBackArrowIcon() { return reinterpret_cast<void*>(SVGPathway::Get(BACK_ARROW_ICON_KEY, TextureStyle::PADDED).lock()->_glTextureID); }
