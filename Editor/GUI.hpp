#pragma once

#include <optional>

#include <imgui.h>

#include <Project.hpp>
#include <ImGuiRenderCommand.hpp>
#include <Core.hpp>

#include "SVGPathway.hpp"

#include "MenuBar.hpp"
#include "SceneHierarchy.hpp"
#include "Observer.hpp"
#include "Console.hpp"
#include "AssetManager.hpp"
#include "SceneViewport.hpp"
#include "GameViewport.hpp"
#include "SceneSettings.hpp"

struct GUI {

	static constexpr auto SCENE_HIERARCHY_TITLE{ "Scene Hierarchy" };
	static constexpr auto OBSERVER_TITLE{ "Observer" };
	static constexpr auto CONSOLE_TITLE{ "Console" };
	static constexpr auto ASSET_MANAGER_TITLE{ "Asset Manager" };
	static constexpr auto SCENE_VIEWPORT_TITLE{ "Scene Viewport" };
	static constexpr auto GAME_VIEWPORT_TITLE{ "Game Viewport" };
	static constexpr auto SCENE_SETTINGS_TITLE{ "Scene Stats/Settings" };

	static constexpr auto SCENE_HIERARCHY_ID{ "###Scene Hierarchy" };
	static constexpr auto OBSERVER_ID{ "###Observer" };
	static constexpr auto CONSOLE_ID{ "###Console" };
	static constexpr auto ASSET_MANAGER_ID{ "###Asset Manager" };
	static constexpr auto SCENE_VIEWPORT_ID{ "###Scene Viewport" };
	static constexpr auto GAME_VIEWPORT_ID{ "###Game Viewport" };
	static constexpr auto SCENE_SETTINGS_ID{ "###Scene Stats/Settings" };

	static constexpr auto FOLDER_ICON_KEY{ "folder-open" };
	static constexpr auto PROJECT_ICON_KEY{ "neodoa" };
	static constexpr auto SCENE_ICON_KEY{ "diagram-project" };
	static constexpr auto FILE_ICON_KEY{ "file" };
	static constexpr auto BACK_ARROW_ICON_KEY{ "arrow-left" };

	static constexpr auto dockSpaceName{ "DockSpace" };

	ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
	ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };

	CorePtr& core;
	WindowPtr& window;
	std::string defaultWindowName{ "NeoDoa Editor" };
	bool dockspaceOpen{ true };

	std::optional<Project> openProject{ std::nullopt };

	MenuBar mb;
	SceneHierarchy sh;
	Observer obs;
	AssetManager am;
	Console con;
	SceneViewport sv;
	GameViewport gv;
	SceneSettings ss;

	float delta;

	GUI(CorePtr& core) noexcept;

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

	void* GetFolderIcon(TextureSize size = TextureSize::MEDIUM);
	void* GetProjectIcon(TextureSize size = TextureSize::MEDIUM);
	void* GetSceneIcon(TextureSize size = TextureSize::MEDIUM);
	void* GetFileIcon(TextureSize size = TextureSize::MEDIUM);
	void* GetBackArrowIcon(TextureSize size = TextureSize::MEDIUM);

private:
	ImGuiIO* io{ nullptr };
	ImFont* font{ nullptr };
	ImFont* fontBold{ nullptr };

	// TODO REMOVE ME WHEN IMGUI IMPLEMENTS THIS WORKAROUND AS API FUNC.
	void ExecuteDockBuilderFocusWorkAround();
};
