#pragma once

#include <optional>

#include <imgui.h>

#include <Utility/ConstexprConcat.hpp>
#include <Utility/SimpleSocket.hpp>
#include <Utility/UndoRedoStack.hpp>

#include <Engine/Core.hpp>
#include <Engine/Project.hpp>
#include <Engine/ImGuiRenderCommand.hpp>

#include <Editor/EditorMeta.hpp>
#include <Editor/SVGPathway.hpp>
#include <Editor/MetaAssetInfo.hpp>
#include <Editor/KeyboardShortcutHandler.hpp>

#include <Editor/MenuBar.hpp>
#include <Editor/SceneHierarchy.hpp>
#include <Editor/Observer.hpp>
#include <Editor/CodeEditor.hpp>
#include <Editor/Console.hpp>
#include <Editor/AssetManager.hpp>
#include <Editor/SceneViewport.hpp>
#include <Editor/GameViewport.hpp>
#include <Editor/SceneSettings.hpp>
#include <Editor/UndoRedoHistory.hpp>
#include <Editor/SceneViewportCameraSettings.hpp>

#include <Editor/NewAssetModal.hpp>

struct GUI {

    struct Events {
        Event<void(Project&)>    OnProjectLoaded  {};
        Event<void(Project&)>    OnProjectSaved   {};
        Event<void()>            OnProjectUnloaded{};

        Event<void(Assets&)>     OnReimport    {};
        Event<void(AssetHandle)> OnAssetCreated{};
        Event<void(AssetHandle)> OnAssetMoved  {};
        Event<void(AssetHandle)> OnAssetDeleted{};
        Event<void(AssetHandle)> OnAssetOpened {};

        Event<void(Scene&)> OnSceneOpened{};
        Event<void()>       OnSceneClosed{};

        Event<void(Entity)> OnEntityCreated{};
        Event<void(Entity)> OnEntityDeleted{};

        struct SceneHierarchy {
            Event<void(Entity)> OnEntitySelected  {};
            Event<void()>       OnEntityDeselected{};
        } SceneHierarchy{};

        struct AssetManager {
            Event<void(AssetHandle)> OnAssetFocused {};
            Event<void(FNode&)>      OnFolderFocused{};
            Event<void()>            OnFocusLost    {};
        } AssetManager{};
    } Events{};

    struct Shortcuts {
        static constexpr auto NewProjectShortcut{ "Ctrl+Shift+N" };
        static constexpr auto OpenProjectShortcut{ "Ctrl+Shift+O" };
        static constexpr auto SaveProjectShortcut{ "Ctrl+Shift+S" };
        static constexpr auto CloseProjectShortcut{ "Ctrl+Shift+W" };

        static constexpr auto ExitProgramShortcut{ "Alt+F4" };

        static constexpr auto UndoShortcut{ "Ctrl+Z" };
        static constexpr auto RedoShortcut{ "Ctrl+Y" };

        static constexpr auto NewSceneShortcut{ "Ctrl+N" };
        static constexpr auto SaveSceneShortcut{ "Ctrl+S" };
        static constexpr auto CloseSceneShortcut{ "Ctrl+W" };
    };

    static constexpr auto FOLDER_ICON_KEY{ "folder-open" };
    static constexpr auto PROJECT_ICON_KEY{ "neodoa" };
    static constexpr auto SCENE_ICON_KEY{ "cubes-stacked" };
    static constexpr auto COMPONENT_ICON_KEY{ "gear" };
    static constexpr auto VERTEX_SHADER_ICON_KEY{ "circle-v" };
    static constexpr auto TESS_CTRL_SHADER_ICON_KEY{ "circle-t" };
    static constexpr auto TESS_EVAL_SHADER_ICON_KEY{ "square-t" };
    static constexpr auto GEOMETRY_SHADER_ICON_KEY{ "circle-g" };
    static constexpr auto FRAGMENT_SHADER_ICON_KEY{ "circle-f" };
    static constexpr auto COMPUTE_SHADER_ICON_KEY{ "circle-c" };
    static constexpr auto FILE_ICON_KEY{ "file" };
    static constexpr auto BACK_ARROW_ICON_KEY{ "arrow-left" };

    static constexpr auto dockSpaceName{ "DockSpace" };

    ImGuiWindowFlags window_flags{ ImGuiWindowFlags_None };
    ImGuiDockNodeFlags dockspace_flags{ ImGuiDockNodeFlags_None };

    const CorePtr& CORE;
    WindowPtr& window;
    std::string defaultWindowName{ "NeoDoa Editor" };
    bool dockspaceOpen{ true };

    float delta{ 0 };

    explicit GUI(const CorePtr& core) noexcept;

    void Prepare();
    void operator() (float delta);
    void End();

    void CreateNewProject(std::string_view workspace, std::string_view name);
    void SaveProjectToDisk();
    void OpenProjectFromDisk(const std::string& path);
    void CloseProject();

    void CreateNewScene(FNode& folder, std::string_view name);
    void OpenScene(AssetHandle sceneHandle);
    void SaveScene() const;
    void CloseScene();

    bool HasOpenProject() const;
    Project& GetOpenProject() const;
    bool HasOpenScene() const;
    Scene& GetOpenScene();

    MenuBar& GetMenuBar();
    SceneHierarchy& GetSceneHierarchy();
    Observer& GetObserver();
    CodeEditor& GetCodeEditor();
    AssetManager& GetAssetManager();
    Console& GetConsole();
    SceneViewport& GetSceneViewport();
    GameViewport& GetGameViewport();
    SceneSettings& GetSceneSettings();
    UndoRedoHistory& GetUndoRedoHistory();
    SceneViewportCameraSettings& GetSceneViewportCameraSettings();
    const MenuBar& GetMenuBar() const;
    const SceneHierarchy& GetSceneHierarchy() const;
    const Observer& GetObserver() const;
    const CodeEditor& GetCodeEditor() const;
    const AssetManager& GetAssetManager() const;
    const Console& GetConsole() const;
    const SceneViewport& GetSceneViewport() const;
    const GameViewport& GetGameViewport() const;
    const SceneSettings& GetSceneSettings() const;
    const UndoRedoHistory& GetUndoRedoHistory() const;
    const SceneViewportCameraSettings& GetSceneViewportCameraSettings() const;

    ImGuiIO* IO() const;
    ImFont* GetFont() const;
    ImFont* GetFontBold() const;

    void* GetFolderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetProjectIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetSceneIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetComponentIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetVertexShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetTessellationControlShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetTessellationEvaluationShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetGeometryShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetFragmentShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetComputeShaderIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetFileIcon(TextureSize size = TextureSize::MEDIUM) const;
    void* GetBackArrowIcon(TextureSize size = TextureSize::MEDIUM) const;

    void* FindIconForFileType(const FNode& file, TextureSize = TextureSize::MEDIUM) const;
    void* FindIconByName(const std::string_view key, TextureSize size = TextureSize::MEDIUM) const;

    MetaAssetInfo& GetMetaInfoOf(const FNode& file);

    //- Modals -//
    void ShowNewSceneAssetModal(FNode& currentFolder) const;
    void ShowNewComponentAssetModal(FNode& currentFolder) const;
    void ShowNewVertexShaderAssetModal(FNode& currentFolder) const;
    void ShowNewTessellationControlShaderAssetModal(FNode& currentFolder) const;
    void ShowNewTessellationEvaluationShaderAssetModal(FNode& currentFolder) const;
    void ShowNewGeometryShaderAssetModal(FNode& currentFolder) const;
    void ShowNewFragmentShaderAssetModal(FNode& currentFolder) const;
    void ShowNewShaderProgramAssetModal(FNode& currentFolder) const;
    void ShowNewMaterialAssetModal(FNode& currentFolder) const;

    //- Undo/Redo History -//
    UndoRedoStack& GetCommandHistory() noexcept;
    template<typename T, typename ...Args> requires(std::is_base_of_v<ICommand, T>)
    void ExecuteCommand(Args&&... args) noexcept {
        history.Do(std::make_unique<T>(*this, std::forward<Args>(args)...));
    }
    void UndoLastCommand() noexcept;
    void RedoLastCommand() noexcept;
    bool CanUndoLastCommand() const noexcept;
    bool CanRedoLastCommand() const noexcept;

private:
    //- Editor Meta -//
    EditorMeta meta{ *this };

    MenuBar mb{ *this };
    SceneHierarchy sh{ *this };
    Observer obs{ *this };
    CodeEditor ce{ *this };
    AssetManager am{ *this };
    Console con{ *this };
    SceneViewport sv{ *this };
    GameViewport gv{ *this };
    SceneSettings ss{ *this };
    UndoRedoHistory urh{ *this };
    SceneViewportCameraSettings svcs{ *this };

    ImGuiIO* io{ nullptr };
    ImFont* font{ nullptr };
    ImFont* fontBold{ nullptr };

    //- Modals -//
    NewAssetModal nam{ *this };

    //- Shortcuts -//
    KeyboardShortcutHandler shortcutHandler{};

    //- Undo/Redo History -//
    UndoRedoStack history{};

    //- Sockets -//
    std::string projectPath;
    ClientSocket request{ "tcp://localhost:5555", SocketType::Pull };
    ClientSocket reply{ "tcp://localhost:5556", SocketType::Push };

    // TODO REMOVE ME WHEN IMGUI IMPLEMENTS THIS WORKAROUND AS API FUNC.
    void ExecuteDockBuilderFocusWorkAround();

    // -- //
    // Open Scene
    UUID sceneUUID{ UUID::Empty() };
    std::optional<Scene> scene{};
    void OnAssetOpened(AssetHandle asset);
    void OnSceneOpened(Scene& scene);
    void OnSceneClosed();
    void OnReimport(Assets& assets);
    void OnAssetDeleted(AssetHandle asset);
    // -- //
};
