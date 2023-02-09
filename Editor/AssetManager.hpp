#pragma once

#include <functional>

#include <imgui.h>

struct GUI;
struct Assets;
struct FNode;

struct AssetManager {

    static constexpr auto REFRESH_BUTTON_TEXT{ "Refresh" };
    static constexpr auto SELECTED_FOLDER_CONTENT_TITLE_TEXT{ "Assets" };

    std::reference_wrapper<GUI> gui;

    explicit AssetManager(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    bool hasContent{ false };
    Assets* assets{ nullptr };
    FNode* root{ nullptr };
    FNode* selectedNode{ nullptr };
    FNode* currentFolder{ nullptr };
    FNode* deletedNode{ nullptr };

    struct TreeViewSettings {
        float currentWidth = 120.0f;
        float minWidth = 30.0f;
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

    struct NewComponentModal {

        using OnCreateNewComponent = std::function<void(const FNode& file)>;

        static constexpr auto MODAL_TITLE_TEXT{ "Create a new component" };
        static constexpr auto MODAL_CONTENT_TEXT{ "Please enter a name for the new component.\n" };
        static constexpr auto MODAL_BUTTON_TEXT{ "OK" };
        inline static const ImVec2 MODAL_BUTTON_SIZE{ 120, 0 };

        void Activate(FNode* currentFolder, OnCreateNewComponent callback = {});
        void Render();

    private:
        FNode* currentFolder{ nullptr };
        OnCreateNewComponent callback{};
        bool modal_active{ false }, modal_open{ true };
        char compName[128]{ "MyComponent" }; /* we dont need to incur a dyn-alloc with std::string */
    } newComponentModal{};

    void RenderMenuBar();
    void RenderTreeView();
    void RenderTreeViewRecursive(FNode& current);

    void RenderSelectedFolderContent();
    void RenderContextMenu();
    void OpenFileAtFileNode(FNode& file);

    void SetSelectedNode(FNode* node);
    void SetCurrentFolder(FNode* folder);
};

