#include "AssetManager.hpp"

#include <algorithm>
#include <imgui_internal.h>
#include <submodules/detector/detector.hpp>

#include <Engine/Texture.hpp>
#include <Engine/ShaderProgramSerializer.hpp>

#include <Utility/constexpr_concat.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "ImGuiExtensions.hpp"
#include "CodeGenerator.hpp"

AssetManager::AssetManager(GUI& gui) noexcept :
    gui(gui) {
    gui.Events.OnProjectLoaded   += std::bind_front(&AssetManager::OnProjectLoaded  , this);
    gui.Events.OnProjectUnloaded += std::bind_front(&AssetManager::OnProjectUnloaded, this);
}

bool AssetManager::Begin() {
    const GUI& gui = this->gui;
    ImGui::PushID(GUI::ASSET_MANAGER_TITLE);
    std::string title(WindowIcons::ASSET_MANAGER_WINDOW_ICON);
    title.append(GUI::ASSET_MANAGER_TITLE);
    title.append(GUI::ASSET_MANAGER_ID);
    bool visible = ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_MenuBar);
    return visible;
}

void AssetManager::Render() {
    GUI& gui = this->gui;

    RenderMenuBar();

    currentFolderContentSettings.minWidth = currentFolderContentSettings.thumbnailSize + 48 + currentFolderContentSettings.itemPadding;
    currentFolderContentSettings.currentWidth = ImGui::GetContentRegionAvail().x - treeViewSettings.currentWidth;
    Splitter(true, 8.0f, &treeViewSettings.currentWidth, &currentFolderContentSettings.currentWidth, treeViewSettings.minWidth, currentFolderContentSettings.minWidth);

    ImGui::BeginChild("tree", { treeViewSettings.currentWidth, 0 }, true);
    RenderTreeView();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("content", { currentFolderContentSettings.currentWidth, 0 }, true);
    RenderContextMenu();
    RenderSelectedFolderContent();
    ImGui::EndChild();

    if (deletedNode != nullptr) {
        gui.Events.OnAssetDeleted(assets->FindAssetAt(*deletedNode));
        assets->DeleteAsset(assets->FindAssetAt(*deletedNode));
        if (currentFolder == deletedNode) {
            SetCurrentFolder(root);
        }
        if (selectedNode == deletedNode) {
            SetSelectedNode(nullptr);
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
                gui.Events.OnReimport(*assets);
                SetCurrentFolder(root);
            } else {
                DOA_LOG_WARNING("Didn't refresh! No open project.");
            }
        }
        static constexpr int searchBarWidth{ 270 };
        ImGui::PushItemWidth(searchBarWidth);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - searchBarWidth);
        ImGui::InputTextWithHint(
            "",
            ICON_FA_MAGNIFYING_GLASS "  Search an asset...",
            fileFilter.SearchQuery.data(), fileFilter.SearchQuery.size(),
            ImGuiInputTextFlags_AutoSelectAll |
            ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_EscapeClearsAll
        );
        ImGui::EndMenuBar();
    }
}

void AssetManager::RenderTreeView() {
    if (!hasContent) return;
    RenderTreeViewRecursive(*root);
}

void AssetManager::RenderTreeViewRecursive(FNode& current) {
    if (!current.IsDirectory()) return;
    GUI& gui = this->gui;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (&current == currentFolder) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    std::vector<std::reference_wrapper<FNode>> children;
    for (auto& child : current.Children()) {
        if (child.IsDirectory()) { children.push_back(child); }
    }

    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    }

    std::string title{ gui.GetMetaInfoOf(current).fa_icon };
    if (&current == root) {
        title = ICON_FA_FOLDER_TREE " ROOT";
    }
    title.append(" ");
    title.append(current.Name());
    bool expanded = ImGui::TreeNodeEx(title.c_str(), flags);
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        SetCurrentFolder(&current);
    }
    if (expanded) {
        for (const auto& child : children) {
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
        if (fileFilter.SearchQuery[0] == '\0') {
            title.append(" - ROOT: ");
            title.append(currentFolder->Path().string());
        } else {
            title.append(" - SEARCH: ");
            title.append(fileFilter.SearchQuery.data());
        }
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
        }
        cmd.append(currentFolder->AbsolutePath().string());
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
            } else if (mWheelDelta > 0) {
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
        for (auto& child : currentFolder->Children()) {
            visible = fileFilter.CheckVisibility(child);
            if (!visible) { continue; }

            ImGui::PushID(i);

            if (i % columns == 0) {
                ImGui::TableNextRow();
            }
            ImGui::TableSetColumnIndex(i++ % columns);

            void* icon = gui.GetMetaInfoOf(child).GetSVGIcon();

            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
            ImGui::ImageButton(icon, { currentFolderContentSettings.thumbnailSize, currentFolderContentSettings.thumbnailSize });
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                assert(assets->IsAssetExistsAt(child));
                dragDropAssetID = assets->FindAssetAt(child)->ID();
                ImGui::SetDragDropPayload("DND_DEMO_CELL", &dragDropAssetID, sizeof(UUID));

                ImGui::Text(child.FullName().data());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginPopupContextItem()) {

                if (ImGui::MenuItem("Delete")) {
                    deletedNode = &child;
                    ImGui::Separator();
                }

                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered()) {
                static bool mouseClick{ false };
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    OpenFileAtFileNode(child);
                } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    mouseClick = true;
                }
                if (mouseClick && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    SetSelectedNode(&child);
                    mouseClick = false;
                }
            }
            ImGui::PopStyleColor();

            std::string fileName{ child.Name() };

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
        DrawRowsBackground(30); /* display bg for 30 items - more than enough for foreseeable future resolutions */
        for (auto& child : currentFolder->Children()) {
            bool visible = fileFilter.CheckVisibility(child);
            if (!visible) { continue; }

            void* icon = gui.GetMetaInfoOf(child).GetSVGIcon();

            if (ImGui::TreeNodeEx(child.Name().data(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf)) {
                if (ImGui::IsItemHovered()) {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        OpenFileAtFileNode(child);
                    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        SetSelectedNode(&child);
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
    GUI& gui = this->gui.get();
    Assets& assets = *gui.CORE->Assets();

    bool isDisabled = !hasContent;
    if (isDisabled) { ImGui::BeginDisabled(); }
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Create")) {
            // TODO MODAL
            static constexpr auto folder = cat(FileIcons::DIRECTORY_ICON_OPEN, " ", "Folder");
            if (ImGui::MenuItem(folder.c)) {
                currentFolder->CreateChildFolder(FNodeCreationParams{
                    currentFolder->OwningProject(),
                    currentFolder,
                    "New Folder",
                });
                //FNode& newFolder = assets.CreateFolder(*currentFolder, "New Folder");
                //AssetHandle handle = assets.FindAssetAt(newFolder);
                //gui.Events.OnAssetCreated(handle);
            }
            ImGui::Separator();
            static constexpr auto component = cat(FileIcons::COMPONENT_ICON, " ", "Component");
            if (ImGui::MenuItem(component.c)) {
                gui.ShowNewComponentAssetModal(*currentFolder);
            }
            static constexpr auto shader = cat(FileIcons::SHADER_ICON, " ", "Shader");
            if (ImGui::BeginMenu(shader.c)) {
                static constexpr auto vertexShader = cat(FileIcons::VERTEX_SHADER_ICON, " ", "Vertex Shader");
                if (ImGui::MenuItem(vertexShader.c)) {
                    gui.ShowNewVertexShaderAssetModal(*currentFolder);
                }
                constexpr auto tessCtrlShader = cat(FileIcons::TESS_CTRL_SHADER_ICON, " ", "Tessellation Control Shader");
                if (ImGui::MenuItem(tessCtrlShader.c)) {
                    gui.ShowNewTessellationControlShaderAssetModal(*currentFolder);
                }
                constexpr auto tessEvalShader = cat(FileIcons::TESS_EVAL_SHADER_ICON, " ", "Tessellation Evaluation Shader");
                if (ImGui::MenuItem(tessEvalShader.c)) {
                    gui.ShowNewTessellationEvaluationShaderAssetModal(*currentFolder);
                }
                constexpr auto geometryShader = cat(FileIcons::GEOMETRY_SHADER_ICON, " ", "Geometry Shader");
                if (ImGui::MenuItem(geometryShader.c)) {
                    gui.ShowNewGeometryShaderAssetModal(*currentFolder);
                }
                constexpr auto fragmentShader = cat(FileIcons::FRAGMENT_SHADER_ICON, " ", "Fragment Shader");
                if (ImGui::MenuItem(fragmentShader.c)) {
                    gui.ShowNewFragmentShaderAssetModal(*currentFolder);
                }
                constexpr auto computeShader = cat(FileIcons::COMPUTE_SHADER_ICON, " ", "Compute Shader");
                if (ImGui::MenuItem(computeShader.c)) {
                    /*
                    newShaderModal.Activate(
                        currentFolder,
                        Shader::Type::Compute,
                        [&assets](const FNode& file) { assets.Import(file)->Deserialize(); }
                    );
                    */
                }
                ImGui::EndMenu();
            }
            static constexpr auto shaderProgram = cat(FileIcons::SHADER_PROGRAM_ICON, " ", "Shader Program");
            if (ImGui::MenuItem(shaderProgram.c)) {
                gui.ShowNewShaderProgramAssetModal(*currentFolder);
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
    if (isDisabled) { ImGui::EndDisabled(); }
}

void AssetManager::OpenFileAtFileNode(FNode& file) {
    GUI& gui = this->gui;
    if (file.IsDirectory()) {
        currentFolder = &file;
        SetSelectedNode(nullptr);
    } else if (file.IsFile()) {
        AssetHandle handle = assets->FindAssetAt(file);

        gui.Events.OnAssetOpened(handle);
        if (Assets::IsSceneFile(file)) {
            gui.OpenScene(handle);
        } else if (Assets::IsComponentDefinitionFile(file)) {
            //gui.ce.AddTab(handle);
        } else if (Assets::IsShaderFile(file)) {
            //gui.ce.AddTab(handle);
        }
    }
}

void AssetManager::SetSelectedNode(FNode* node) {
    GUI& gui = this->gui;

    selectedNode = node;
    if (node != nullptr) {
        AssetHandle handle = assets->FindAssetAt(*node);
        if (handle) {
            gui.Events.AssetManager.OnAssetFocused(handle);
        } else {
            gui.Events.AssetManager.OnFolderFocused(*node);
        }
    } else {
        gui.Events.AssetManager.OnFocusLost();
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

// Inner struct: FileFilter
bool AssetManager::FileFilter::CheckVisibility(const FNode& file) const {
    if (!file.IsDirectory() && !Owner.get().assets->IsAssetExistsAt(file)) { return false; }
    if (file.Extension() == ".id") { return false; }
    if (file.Extension() == ".doa") { return false; }
    if (file.FullName() == "metaAssetInfo.bank") { return false; }
    if (file.FullName() == "imgui.ini") { return false; }

    /* if query is empty, there is no query, hence no search is needed, all is shown */
    if (SearchQuery[0] == '\0') { return true; }

    /* query is not present in the string, as whole, thus search failed, hide this file */
    if (file.FullName().find(SearchQuery.data()) == std::string::npos) { return false; }

    return true;
}

void AssetManager::OnProjectLoaded(Project& project) {
    hasContent = true;
    assets = gui.get().CORE->Assets().get();
    root = &assets->Root();
    SetCurrentFolder(root);
}
void AssetManager::OnProjectUnloaded() {
    hasContent = false;
    assets = nullptr;
    root = nullptr;
    SetCurrentFolder(root);
}