#pragma once

#include <optional>

#include <imgui.h>

#include <Project.hpp>
#include <ImGuiRenderCommand.hpp>
#include <Core.hpp>

#include "MenuBar.hpp"
#include "SceneHierarchy.hpp"
#include "Observer.hpp"
#include "Console.hpp"
#include "AssetManager.hpp"

struct GUI {

	static constexpr const char* const SCENE_HIERARCHY_ID{ "Scene Hierarchy" };
	static constexpr const char* const OBSERVER_ID{ "Observer" };
	static constexpr const char* const CONSOLE_ID{ "Console" };
	static constexpr const char* const ASSET_MANAGER_ID{ "Asset Manager" };

	static constexpr const char* const FOLDER_ICON_KEY{ "folder-open" };
	static constexpr const char* const PROJECT_ICON_KEY{ "neodoa" };
	static constexpr const char* const SCENE_ICON_KEY{ "diagram-project" };
	static constexpr const char* const FILE_ICON_KEY{ "file" };
	static constexpr const char* const BACK_ARROW_ICON_KEY{ "arrow-left" };

	ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
	ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };
	bool opt_fullscreen{ true };

	std::unique_ptr<Core>& core;
	std::string defaultWindowName{ "NeoDoa Editor" };
	bool dockspaceOpen{ true };

	std::optional<Project> openProject{ std::nullopt };

	MenuBar mb;
	SceneHierarchy sh;
	Observer obs;
	Console con;
	AssetManager am;

	float delta;

	GUI(std::unique_ptr<Core>& core) noexcept;

	void Prepare();
	void operator() (float delta);
	void End();

	void CreateNewProject(std::string_view workspace, std::string_view name);
	void SaveProjectToDisk();
	void OpenProjectFromDisk(const std::string& path);
	void CloseProject();

	void CreateNewScene(std::string_view relativePath, std::string_view name);

	bool HasOpenProject();
	bool HasOpenScene();

	ImGuiIO* IO();
	ImFont* GetFont();
	ImFont* GetFontBold();

	void* GetFolderIcon();
	void* GetProjectIcon();
	void* GetSceneIcon();
	void* GetFileIcon();
	void* GetBackArrowIcon();

private:
	ImGuiIO* io{ nullptr };
	ImFont* font{ nullptr };
	ImFont* fontBold{ nullptr };
};
