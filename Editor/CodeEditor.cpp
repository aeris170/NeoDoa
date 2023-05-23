#include "CodeEditor.hpp"

#include "GUI.hpp"
#include "Icons.hpp"
#include "FileNode.hpp"

#include <imgui_internal.h>

CodeEditor::CodeEditor(GUI& gui) noexcept :
    gui(gui) {
    tabs.reserve(10);
    emptyTab.textEditor.SetReadOnly(true);
}

bool CodeEditor::Begin() {
    GUI& gui = this->gui;

    ImGui::PushID(GUI::CODE_EDITOR_TITLE);
    std::string title(WindowIcons::CODE_EDITOR_WINDOW_ICON);
    title.append(GUI::CODE_EDITOR_TITLE);
    title.append(GUI::CODE_EDITOR_ID);
    bool visible = ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

    return visible;
}
void CodeEditor::Render() {
    GUI& gui = this->gui;
    RenderMenuBar();

    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_AutoSelectNewTabs |
        ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_Reorderable;
    if(ImGui::BeginTabBar("#CodeEditorOpenTabs", tabBarFlags)) {
        for (auto i = 0; i < tabs.size(); i++) {
            if (i == removedTabIndex) { continue; }

            auto& tab = tabs[i];
            ImGuiTabItemFlags flags{ ImGuiTabItemFlags_None };
            if (tab.IsUnsaved()) {
                flags |= ImGuiTabItemFlags_UnsavedDocument;
            }
            if (i == selectedTabIndex) {
                flags |= ImGuiTabItemFlags_SetSelected;
            }
            bool tabActive = ImGui::BeginTabItem(tabs[i].tabName.c_str(), nullptr, flags);
            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    selectedTabIndex = i;
                }
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
                    CloseTabAt(i);
                }
            }
            if (tabActive) {
                tab.Render();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    if (tabs.empty()) {
        emptyTab.Render();
    }
}
void CodeEditor::End() {
    ImGui::End();
    ImGui::PopID();

    if (removedTabIndex != -1) {
        tabs.erase(tabs.begin() + removedTabIndex);
        removedTabIndex = -1;
    }
}

void CodeEditor::CloseTabAt(int index) {
    removedTabIndex = index;
    if (removedTabIndex <= selectedTabIndex) {
        selectedTabIndex = std::max(0, selectedTabIndex--);
    }
}

void CodeEditor::RenderMenuBar() {
    bool menuBarEnabled = selectedTabIndex != -1;
    if (menuBarEnabled) {
        bool shortcutsEnabled = menuBarEnabled;
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S) && shortcutsEnabled) {
            tabs[selectedTabIndex].SaveChanges();
        }
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S) && shortcutsEnabled) {
            SaveAllChanges();
        }
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Q) && shortcutsEnabled) {
            CloseTabAt(selectedTabIndex);
        }
    }
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl-S", nullptr, menuBarEnabled)) {
                tabs[selectedTabIndex].SaveChanges();
            }
            if (ImGui::MenuItem("Save All", "Ctrl-Shift-S", nullptr, menuBarEnabled)) {
                SaveAllChanges();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", "Ctrl-Q", nullptr, menuBarEnabled)) {
                CloseTabAt(selectedTabIndex);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            TextEditor* te = menuBarEnabled ? &tabs[selectedTabIndex].textEditor : nullptr;
            menuBarEnabled &= te != nullptr;
            if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, menuBarEnabled)) {
                te->Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, menuBarEnabled)) {
                te->Redo();
            }
            ImGui::Separator();
            bool editorHasSelection = te ? te->HasSelection() : false;
            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, menuBarEnabled && editorHasSelection)) {
                te->Copy();
            }
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, menuBarEnabled && editorHasSelection)) {
                te->Cut();
            }
            if (ImGui::MenuItem("Delete", "Del", nullptr, menuBarEnabled && editorHasSelection)) {
                te->Delete();
            }
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, menuBarEnabled && ImGui::GetClipboardText() != nullptr)) {
                te->Paste();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr, menuBarEnabled)) {
                te->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(te->GetTotalLines(), 0));
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Set Dark palette")) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetPalette(TextEditor::GetDarkPalette());
                }
            }
            if (ImGui::MenuItem("Set Light palette")) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetPalette(TextEditor::GetLightPalette());
                }
            }
            if (ImGui::MenuItem("Set Retro blue palette")) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetPalette(TextEditor::GetRetroBluePalette());
                }
            }
            ImGui::Separator();
            if (ImGui::Checkbox("Colored Syntax", &isSyntaxColoringEnabled)) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetColorizerEnable(isSyntaxColoringEnabled);
                }
            }
            if (ImGui::Checkbox("Short Tab Glyphs", &isShortTabGlyphsVisible)) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetShowShortTabGlyphs(isShortTabGlyphsVisible);
                }
            }
            if (ImGui::Checkbox("Whitespaces", &isWhitespaceVisible)) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetShowWhitespaces(isWhitespaceVisible);
                }
            }
            ImGui::PushItemWidth(120);
            if (ImGui::SliderInt("Tab Size", &tabSize, 1, 4)) {
                for (auto& tab : tabs) {
                    tab.textEditor.SetTabSize(tabSize);
                }
            }
            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void CodeEditor::AddTab(AssetHandle assetHandle) {
    if (!assetHandle.HasValue()) { return; }

    auto search = std::ranges::find_if(tabs, [&assetHandle](EditorTab& element) {
        return element.currentAsset == assetHandle;
    });
    if (search != tabs.end()) {
        selectedTabIndex = search - tabs.begin();
        return;
    }

    tabs.emplace_back(assetHandle, isSyntaxColoringEnabled, isShortTabGlyphsVisible, isWhitespaceVisible, tabSize);
    selectedTabIndex++;
}

void CodeEditor::SaveAllChanges() {
    for (auto& tab : tabs) {
        tab.SaveChanges();
    }
}

// Inner-struct: EditorTab
CodeEditor::EditorTab::EditorTab(AssetHandle currentAsset, bool isSyntaxColoringEnabled, bool isShortTabGlyphsVisible, bool isWhitespaceVisible, int tabSize) noexcept :
    currentAsset(currentAsset) {
    textEditor.SetColorizerEnable(isSyntaxColoringEnabled);
    textEditor.SetShowShortTabGlyphs(isShortTabGlyphsVisible);
    textEditor.SetShowWhitespaces(isWhitespaceVisible);
    textEditor.SetTabSize(tabSize);
    currentAsset->File().ReadContent();
    unsavedText = currentAsset->File().DisposeContent();
    textEditor.SetText(unsavedText);
    textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::NeoDoaComponentDefinition());
    tabName = currentAsset->File().Name();
    tabName.append("###");
    tabName.append(currentAsset->File().AbsolutePath().string());
}

bool CodeEditor::EditorTab::IsUnsaved() const { return unsavedText != textEditor.GetText(); }
void CodeEditor::EditorTab::SaveChanges() {
    unsavedText = textEditor.GetText();
    currentAsset->File().ModifyContent(unsavedText);
    currentAsset->ForceDeserialize();
}

void CodeEditor::EditorTab::Render() {
    textEditor.Render(tabName.c_str(), false, { 0, -ImGui::GetTextLineHeight() });
    auto cpos = textEditor.GetCursorPosition();
    ImGui::Text(
        "Ln:%d   Ch:%d   |   %d lines   |   %s   |   %s   |   %s",
        cpos.mLine + 1,
        cpos.mColumn + 1,
        textEditor.GetTotalLines(),
        textEditor.IsOverwrite() ? "Ovr" : "Ins",
        textEditor.GetLanguageDefinitionName(),
        currentAsset.HasValue() ? currentAsset->File().Path().string().c_str() : "Empty Document"
    );
}
