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
        EditorTab(AssetHandle currentAsset, bool isSyntaxColoringEnabled, bool isShortTabGlyphsVisible, bool isWhitespaceVisible, int tabSize) noexcept;

        bool IsUnsaved() const;
        void SaveChanges();

    private:
        void Render();

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

    void AddTab(AssetHandle assetHandle);

    void SaveAllChanges();

private:
    bool isSyntaxColoringEnabled{ true };
    bool isShortTabGlyphsVisible{ false };
    bool isWhitespaceVisible{ true };
    int tabSize{ 4 };

    int selectedTabIndex{ -1 };
    std::vector<EditorTab> tabs{};
    EditorTab emptyTab{};

    int removedTabIndex{ -1 };

    void CloseTabAt(int index);

    void RenderMenuBar();
};
