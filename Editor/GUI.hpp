#pragma once

#include <optional>

#include <imgui.h>

#include <Engine/Core.hpp>
#include <Engine/Project.hpp>
#include <Engine/ImGuiRenderCommand.hpp>

#include <Editor/SVGPathway.hpp>
#include <Editor/FileDialog.hpp>
#include <Editor/MetaAssetInfo.hpp>

#include <Editor/MenuBar.hpp>
#include <Editor/SceneHierarchy.hpp>
#include <Editor/Observer.hpp>
#include <Editor/CodeEditor.hpp>
#include <Editor/Console.hpp>
#include <Editor/AssetManager.hpp>
#include <Editor/SceneViewport.hpp>
#include <Editor/GameViewport.hpp>
#include <Editor/SceneSettings.hpp>

struct GUI {

    static constexpr auto SCENE_HIERARCHY_TITLE{ "Scene Hierarchy" };
    static constexpr auto OBSERVER_TITLE{ "Observer" };
    static constexpr auto CODE_EDITOR_TITLE{ "Code Editor" };
    static constexpr auto CONSOLE_TITLE{ "Console" };
    static constexpr auto ASSET_MANAGER_TITLE{ "Asset Manager" };
    static constexpr auto SCENE_VIEWPORT_TITLE{ "Scene Viewport" };
    static constexpr auto GAME_VIEWPORT_TITLE{ "Game Viewport" };
    static constexpr auto SCENE_SETTINGS_TITLE{ "Scene Stats/Settings" };

    static constexpr auto SCENE_HIERARCHY_ID{ "###Scene Hierarchy" };
    static constexpr auto OBSERVER_ID{ "###Observer" };
    static constexpr auto CODE_EDITOR_ID{ "###Code Editor" };
    static constexpr auto CONSOLE_ID{ "###Console" };
    static constexpr auto ASSET_MANAGER_ID{ "###Asset Manager" };
    static constexpr auto SCENE_VIEWPORT_ID{ "###Scene Viewport" };
    static constexpr auto GAME_VIEWPORT_ID{ "###Game Viewport" };
    static constexpr auto SCENE_SETTINGS_ID{ "###Scene Stats/Settings" };

    static constexpr auto FOLDER_ICON_KEY{ "folder-open" };
    static constexpr auto PROJECT_ICON_KEY{ "neodoa" };
    static constexpr auto SCENE_ICON_KEY{ "cubes-stacked" };
    static constexpr auto COMPONENT_ICON_KEY{ "gear" };
    static constexpr auto FILE_ICON_KEY{ "file" };
    static constexpr auto BACK_ARROW_ICON_KEY{ "arrow-left" };

    static constexpr auto dockSpaceName{ "DockSpace" };

    ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
    ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };

    const CorePtr& CORE;
    WindowPtr& window;
    std::string defaultWindowName{ "NeoDoa Editor" };
    bool dockspaceOpen{ true };

    MenuBar mb{ *this };
    SceneHierarchy sh{ *this };
    Observer obs{ *this };
    CodeEditor ce{ *this };
    AssetManager am{ *this };
    Console con{ *this };
    SceneViewport sv{ *this };
    GameViewport gv{ *this };
    SceneSettings ss{ *this };

    float delta{ 0 };

    explicit GUI(const CorePtr& core) noexcept;

    void Prepare();
    void operator() (float delta);
    void End();

    void CreateNewProject(std::string_view workspace, std::string_view name);
    void SaveProjectToDisk();
    void OpenProjectFromDisk(const std::string& path);
    void CloseProject();

    void CreateNewScene(std::string_view relativePath, std::string_view name);

    bool HasOpenProject() const;
    Project& GetOpenProject() const;
    bool HasOpenScene() const;
    Scene& GetOpenScene() const;

    ImGuiIO* IO() const;
    ImFont* GetFont() const;
    ImFont* GetFontBold() const;

    void* GetFolderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetProjectIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetSceneIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetComponentIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetFileIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetBackArrowIcon(TextureSize size = TextureSize::MEDIUM) const;

    void* FindIconForFileType(const FNode& file, TextureSize = TextureSize::MEDIUM) const;
    void* FindIconByName(const std::string_view key, TextureSize size = TextureSize::MEDIUM) const;

    MetaAssetInfo& GetMetaInfoOf(const FNode& file);
    void ReloadMetaInfo();

private:
    ImGuiIO* io{ nullptr };
    ImFont* font{ nullptr };
    ImFont* fontBold{ nullptr };

    MetaAssetInfoBank metaInfo{};

    // TODO REMOVE ME WHEN IMGUI IMPLEMENTS THIS WORKAROUND AS API FUNC.
    void ExecuteDockBuilderFocusWorkAround();
};
