#pragma once

#include <array>
#include <format>
#include <functional>

#include <imgui.h>

#include <Engine/Shader.hpp>

struct GUI;
struct Assets;
struct FNode;
struct Project;

struct AssetManager {

    static constexpr auto REFRESH_BUTTON_TEXT{ "Refresh" };
    static constexpr auto SELECTED_FOLDER_CONTENT_TITLE_TEXT{ "Assets" };

    std::reference_wrapper<GUI> gui;

    explicit AssetManager(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

    FNode* GetCurrentFolder();

private:
    bool hasContent{ false };
    Assets* assets{ nullptr };
    FNode* root{ nullptr };
    FNode* selectedNode{ nullptr };
    FNode* currentFolder{ nullptr };
    FNode* deletedNode{ nullptr };
    UUID dragDropAssetID{};

    struct TreeViewSettings {
        float currentWidth = 120.0f;
        float minWidth = 1.0f;
        int selectedElemIndex = 0;
    } treeViewSettings{};

    struct CurrentFolderContentSettings {
        float currentWidth = 300.0f;
        float minWidth = 120.0f;

        float itemPadding = 16.0f;
        float thumbnailSize = 64.0f;
        const float thumbnailMinSize = 48.0f;
        const float thumbnailMaxSize = 96.0f;
        int maxTextLine = 3;
        enum class ViewMode {
            Icons,
            List
        } viewMode = ViewMode::Icons;
    } currentFolderContentSettings{};

    struct FileFilter {
        std::reference_wrapper<AssetManager> Owner;
        std::array<char, 32> SearchQuery{};
        bool HasSearchQuery() const;
        bool CheckVisibility(const FNode& file) const;
    } fileFilter{ *this };

    void RenderMenuBar();
    void RenderTreeView();
    void RenderTreeViewRecursive(FNode& current);

    void RenderSelectedFolderContent();
    void RenderContextMenu();
    void OpenFileAtFileNode(FNode& file);

    void SetSelectedNode(FNode* node);
    void SetCurrentFolder(FNode* folder);

    void OnProjectLoaded(Project& project);
    void OnProjectUnloaded();
};

