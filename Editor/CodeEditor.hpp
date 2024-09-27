#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_set>

#include <imgui.h>

#include <Engine/Assets.hpp>

#include <Editor/TextEditor.hpp>

struct GUI;

struct CodeEditor {

    struct EditorTab {
        EditorTab() noexcept = default;
        EditorTab(AssetHandle currentAsset, bool isSyntaxColoringEnabled, bool isWhitespaceVisible, int tabSize) noexcept;

        bool IsUnsaved() const;
        void SaveChanges();

    private:
        void Render();

        UUID currentAssetID{};
        AssetHandle currentAsset{};
        std::string unsavedText{};
        TextEditor textEditor{};
        std::string tabName{"EmptyCodeEditorTab###EmptyCodeEditorTab"};

        friend struct CodeEditor;
    };

    static inline ImVec2 EDITOR_WINDOW_DEFAULT_SIZE{ 600.f, 600.f };

    std::reference_wrapper<GUI> gui;

    explicit CodeEditor(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

    void SaveAllChanges();

private:
    bool isSyntaxColoringEnabled{ true };
    bool isWhitespaceVisible{ true };
    int tabSize{ 4 };

    size_t selectedTabIndex{ std::numeric_limits<size_t>::max() };
    std::vector<EditorTab> tabs{};
    EditorTab emptyTab{};

    size_t removedTabIndex{ std::numeric_limits<size_t>::max() };

    void AddTab(AssetHandle assetHandle);
    void CloseTabAt(size_t index);

    void RenderMenuBar();

    void OnReimport(Assets& assets);
    void OnAssetDeleted(AssetHandle handle);
    void OnAssetOpened(AssetHandle handle);
};
