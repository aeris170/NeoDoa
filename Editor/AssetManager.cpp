#include "AssetManager.hpp"

#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <detector.hpp>

#include <Texture.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "ImGuiExtensions.hpp"

AssetManager::AssetManager(GUI& gui) noexcept :
    gui(gui) {}

void AssetManager::Begin() {
    GUI& gui = this->gui;
    ImGui::PushID(gui.ASSET_MANAGER_TITLE);
    std::string title(WindowIcons::ASSET_MANAGER_WINDOW_ICON);
    title.append(gui.ASSET_MANAGER_TITLE);
    title.append(gui.ASSET_MANAGER_ID);
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

    hasContent = gui.HasOpenProject();
    if (hasContent) {
        assets = &gui.openProject->Assets();
        root = &assets->Root();
        if (currentFolder == nullptr) {
            SetCurrentFolder(root);
        }
    } else {
        assets = nullptr;
        root = nullptr;
        SetCurrentFolder(root);
    }
}

void AssetManager::Render() {
    GUI& gui = this->gui;

    RenderMenuBar();

    currentFolderContentSettings.minWidth = currentFolderContentSettings.thumbnailSize + 48 + currentFolderContentSettings.itemPadding;
    currentFolderContentSettings.currentWidth = ImGui::GetContentRegionAvail().x - treeViewSettings.currentWidth;
    Splitter(true, 8.0f, &treeViewSettings.currentWidth, &currentFolderContentSettings.currentWidth, treeViewSettings.minWidth, currentFolderContentSettings.minWidth);

    ImGui::BeginChild("test", { treeViewSettings.currentWidth, 0 }, true);
    RenderTreeView();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("test2", { currentFolderContentSettings.currentWidth, 0 }, true);
    RenderContextMenu();
    RenderSelectedFolderContent();
    ImGui::EndChild();

    if (deletedNode != nullptr) {
        deletedNode->ParentNode()->DeleteChildNode(deletedNode);
        if (currentFolder == deletedNode) {
            SetCurrentFolder(root);
        }
        deletedNode = nullptr;
    }
}

void AssetManager::End() {
    ImGui::End();
    ImGui::PopID();
}

void AssetManager::RenderMenuBar() {
    GUI& gui = this->gui;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::MenuItem(REFRESH_BUTTON_TEXT)) {
            if (hasContent) {
                assets->ReimportAll();
                SetCurrentFolder(root);
            } else {
                DOA_LOG_WARNING("Didn't refresh! No open project.");
            }
        }
        ImGui::EndMenuBar();
    }
}

void AssetManager::RenderTreeView() {
    if (!hasContent) return;
    RenderTreeViewRecursive(root);
}

void AssetManager::RenderTreeViewRecursive(FNode* current) {
    if (!current->IsDirectory()) return;
    GUI& gui = this->gui;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    std::vector<FNode*> children;
    for (auto& child : current->Children()) {
        if (child->IsDirectory()) { children.push_back(child); }
    }

    if (children.size() == 0) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    }

    std::string title(ICON_FA_FOLDER " ");
    if (current == root) {
        title = ICON_FA_FOLDER_TREE " ROOT";
    }
    title.append(current->Name());
    if (ImGui::TreeNodeEx(title.c_str(), flags)) {
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            SetCurrentFolder(current);
        }

        for (auto& child : children) {
            RenderTreeViewRecursive(child);
        }
        ImGui::TreePop();
    }
}

void AssetManager::RenderSelectedFolderContent() {
    GUI& gui = this->gui;
    auto pos = ImGui::GetCursorPos();
    ImGui::Text(SELECTED_FOLDER_CONTENT_TITLE_TEXT);
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = { ImGui::GetItemRectMin().x + ImGui::GetContentRegionAvail().x, ImGui::GetItemRectMax().y };
    ImGui::GetWindowDrawList()->AddRectFilled({ min.x - 2, min.y - 2 }, { max.x + 2, max.y + 2 }, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TitleBg)));
    ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Header)));

    ImGui::SetCursorPos({ pos.x - 2, pos.y - 3 });
    if (!hasContent) ImGui::BeginDisabled();
    ImGui::PushClipRect(min, max, true);
    //ImGui::PushStyleColor(ImGuiCol_Button, { 0.38f, 0.38f, 0.38f, 1.0f }); // TODO remove these magic colors
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
    ImGui::Button(ICON_FA_ARROW_LEFT, { ImGui::CalcTextSize(ICON_FA_ARROW_LEFT).x * 2, max.y - min.y + 4 });
    ImGui::PopStyleColor(1);
    ImGui::PopClipRect();
    if (!hasContent) ImGui::EndDisabled();
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        SetCurrentFolder(currentFolder->ParentNode() ? currentFolder->ParentNode() : root);
    }

    ImGui::SameLine();

    ImGui::SetCursorPos({ ImGui::GetCursorPos().x - 1, ImGui::GetCursorPos().y - 1 });
    std::string title(AssetManagerIcons::SELECTED_FOLDER_CONTENT_TITLE_ICON);
    title.append(SELECTED_FOLDER_CONTENT_TITLE_TEXT);
    if (currentFolder != nullptr) {
        title.append(" - ROOT: ");
        title.append(currentFolder->Path().string());
    }
    ImGui::PushFont(gui.GetFontBold());
    ImGui::Text(title.c_str());
    ImGui::PopFont();
    if (currentFolder == nullptr) return;
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        std::string cmd;
        if constexpr (detect::is_windows_v) {
            cmd = "explorer ";
        } else if constexpr (detect::is_macos_v) {
            cmd = "open ";
        } else if constexpr (detect::is_linux_v) {
            cmd = "nautilus --browser "; // linux not supported
        } else {
            throw; // platform not supported
        }
        cmd.append(currentFolder->Path().string());
        std::system(cmd.c_str());
    }

    ImGui::NewLine();

    if (ImGui::GetIO().KeyCtrl) {
        float mWheelDelta = gui.IO()->MouseWheel;
        if (currentFolderContentSettings.thumbnailSize == currentFolderContentSettings.thumbnailMinSize) {
            if (mWheelDelta < 0) {
                if (currentFolderContentSettings.viewMode != CurrentFolderContentSettings::ViewMode::List) {
                    currentFolderContentSettings.viewMode = CurrentFolderContentSettings::ViewMode::List;
                }
            } else if (mWheelDelta > 0 && currentFolderContentSettings.viewMode != CurrentFolderContentSettings::ViewMode::Icons) {
                if (currentFolderContentSettings.viewMode != CurrentFolderContentSettings::ViewMode::Icons) {
                    currentFolderContentSettings.viewMode = CurrentFolderContentSettings::ViewMode::Icons;
                    mWheelDelta = 0;
                }
            }
        }
        currentFolderContentSettings.thumbnailSize += mWheelDelta;

        currentFolderContentSettings.thumbnailSize = std::clamp(currentFolderContentSettings.thumbnailSize, currentFolderContentSettings.thumbnailMinSize, currentFolderContentSettings.thumbnailMaxSize);
        currentFolderContentSettings.itemPadding = currentFolderContentSettings.thumbnailSize / 4;
    }

    if (currentFolderContentSettings.viewMode == CurrentFolderContentSettings::ViewMode::Icons) {
        float cell = currentFolderContentSettings.thumbnailSize + currentFolderContentSettings.itemPadding;
        float width = ImGui::GetContentRegionAvail().x;
        currentFolderContentSettings.minWidth = cell;

        int columns = static_cast<int>(width / cell);
        if (columns < 1) return;

        bool visible = ImGui::BeginTable("currentFolder", columns);
        if (!visible) return;

        int i = 0;
        ImGui::TableNextRow();
        for (auto child : currentFolder->Children()) {
            if (!child->IsDirectory() && !assets->IsAssetExistsAt(*child)) { continue; }

            ImGui::PushID(i);

            if (i % columns == 0) {
                ImGui::TableNextRow();
            }
            ImGui::TableSetColumnIndex(i++ % columns);

            void* icon = gui.FindIconForFileType(*child);

            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
            ImGui::ImageButton(icon, { currentFolderContentSettings.thumbnailSize, currentFolderContentSettings.thumbnailSize });
            if (ImGui::BeginPopupContextItem()) {

                if (ImGui::MenuItem("Delete")) {
                    deletedNode = child;
                    ImGui::Separator();
                }

                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    OpenFileAtFileNode(child);
                } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    SetSelectedNode(child);
                }
            }
            ImGui::PopStyleColor();

            std::string fileName = child->Name();

            float textHeight = ImGui::GetTextLineHeight();
            auto textSize = ImGui::CalcTextSize(fileName.c_str(), nullptr, true, currentFolderContentSettings.thumbnailSize);
            bool shouldShorten = textSize.y / textHeight > currentFolderContentSettings.maxTextLine;
            if (shouldShorten) {
                auto begin = fileName.begin();
                auto end = --fileName.end();
                std::string shortened;
                do {
                    shortened = std::string(begin, end + 1);
                    shortened.append("...");
                    textSize = ImGui::CalcTextSize(shortened.c_str(), nullptr, true, currentFolderContentSettings.thumbnailSize);
                    do {
                        --end;
                    } while (end[0] == ' ');
                } while (textSize.y / textHeight > currentFolderContentSettings.maxTextLine);
                fileName = shortened;
            }

            ImGui::PushClipRect(ImGui::GetItemRectMin(), { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + (currentFolderContentSettings.maxTextLine + 0.5f) * textHeight }, true);
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + currentFolderContentSettings.thumbnailSize);
            ImGui::PushFont(gui.GetFontBold());
            ImGui::TextUnformatted(fileName.c_str());
            ImGui::PopFont();
            ImGui::PopTextWrapPos();
            ImGui::PopClipRect();

            ImGui::PopID();
        }

        ImGui::EndTable();
    } else if (currentFolderContentSettings.viewMode == CurrentFolderContentSettings::ViewMode::List) {
        for (auto child : currentFolder->Children()) {
            if (!child->IsDirectory() && !assets->IsAssetExistsAt(*child)) { continue; }

            void* icon = gui.FindIconForFileType(*child);

            if (ImGui::TreeNodeEx(child->Name().c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf)) {
                if (ImGui::IsItemHovered()) {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        OpenFileAtFileNode(child);
                    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        SetSelectedNode(child);
                    }
                }

                ImGui::TreePop();
            }

            float textHeight = ImGui::GetTextLineHeight();
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = { min.x + textHeight, min.y + textHeight };
            ImGui::GetWindowDrawList()->AddImage(icon, min, max);
        }
    }
}

void AssetManager::RenderContextMenu() {
    bool isDisabled = !hasContent;
    if (isDisabled) { ImGui::BeginDisabled(); }
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Folder")) {
                currentFolder->CreateChildFolder(FNodeCreationParams{
                    currentFolder->OwningProject(),
                    currentFolder,
                    "New Folder",
                    "",
                    "",
                    true
                                                 });
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
    if (isDisabled) { ImGui::EndDisabled(); }
}

void AssetManager::OpenFileAtFileNode(FNode* file) {
    GUI& gui = this->gui;
    if (file->IsDirectory()) {
        currentFolder = file;
        SetSelectedNode(nullptr);
    } else if (file->IsFile()) {
        if (Assets::IsSceneFile(file)) {
            gui.openProject->OpenScene(assets->FindAssetAt(*file));
        }
    }
}

void AssetManager::SetSelectedNode(FNode* node) {
    GUI& gui = this->gui;

    selectedNode = node;
    if (node == nullptr) {
        gui.obs.ResetDisplayTarget();
    } else {
        gui.obs.SetDisplayTarget(selectedNode);
    }
}
void AssetManager::SetCurrentFolder(FNode* folder) {
    if (folder == nullptr) {
        currentFolder = nullptr;
        SetSelectedNode(nullptr);
        return;
    }

    if (!folder->IsDirectory()) { return; }
    GUI& gui = this->gui;

    if (folder == root || folder == currentFolder->ParentNode()) {
        SetSelectedNode(nullptr);
    } else {
        SetSelectedNode(folder);
    }
    currentFolder = folder;
}