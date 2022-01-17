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

	static constexpr const char* const FOLDER_ICON_KEY{ "!!folderIcon!!" };
	static constexpr const char* const FILE_ICON_KEY{ "!!fileIcon!!" };

	static constexpr const char* const SCENE_ICON = ICON_FA_PROJECT_DIAGRAM " ";
	static constexpr const char* const ENTITY_ICON = ICON_FA_CUBE " ";
	static constexpr const char* const GENERIC_SCRIPT_ICON = ICON_FA_FILE_CODE " "; // or ICON_FA_COG
	static const std::unordered_map<std::string, const std::string> CUSTOM_SCRIPT_ICONS;

	ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
	ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };
	bool opt_fullscreen{ true };

	std::unique_ptr<Core>& core;
	std::string defaultWindowName{ "NeoDoa Editor" };
	bool dockspaceOpen{ true };
	std::unordered_map<std::string, std::weak_ptr<Texture>> texIcons;

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

	void* GetFolderIcon();
	void* GetFileIcon();

private:
	ImGuiIO* io{ nullptr };
	ImFont* font{ nullptr };
};
