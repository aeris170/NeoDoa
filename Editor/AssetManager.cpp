#include "AssetManager.hpp"

#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <detector.hpp>

#include <Texture.hpp>

#include "GUI.hpp"
#include "ImGuiExtensions.hpp"

AssetManager::AssetManager(GUI* gui) noexcept :
	gui(gui) {}

void AssetManager::Begin() {
	ImGui::PushID(gui->ASSET_MANAGER_ID);
	ImGui::Begin(gui->ASSET_MANAGER_ID, nullptr, ImGuiWindowFlags_MenuBar);

	hasContent = gui->HasOpenProject();
	if(hasContent) {
		assets = &gui->openProject->Assets();
		root = &assets->Root();
		if (selectedFolder == nullptr) {
			selectedFolder = root;
		}
	} else {
		assets = nullptr;
		root = nullptr;
	}
}

void AssetManager::Render() {
	RenderMenuBar();

	selectedFolderContentSettings.minWidth = selectedFolderContentSettings.thumbnailSize + 48 + selectedFolderContentSettings.itemPadding;
	selectedFolderContentSettings.currentWidth = ImGui::GetContentRegionAvail().x - treeViewSettings.currentWidth;
	Splitter(true, 8.0f, &treeViewSettings.currentWidth, &selectedFolderContentSettings.currentWidth, treeViewSettings.minWidth, selectedFolderContentSettings.minWidth);

	ImGui::BeginChild("test", { treeViewSettings.currentWidth, 0 }, true);
	RenderTreeView();
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("test2", { selectedFolderContentSettings.currentWidth, 0 }, true);
	RenderSelectedFolderContent();
	ImGui::EndChild();
}

void AssetManager::End() {
	ImGui::End();
	ImGui::PopID();
}

void AssetManager::RenderMenuBar() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::MenuItem(REFRESH_BUTTON_TEXT)) {
			if (hasContent) {
				assets->ReScan();
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
	if (!current->_isDir) return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere;

	std::vector<FNode*> children;
	for (auto& child : current->_children) {
		if (child._isDir) { children.push_back(&child); }
	}

	if (children.size() == 0) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
	}

	std::string title(ICON_FA_FOLDER " ");
	if (current == root) {
		title = ICON_FA_FOLDER_TREE " ";
	}
	title.append(current->_name);
	if (ImGui::TreeNodeEx(title.c_str(), flags)) {
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			selectedFolder = current;
		}

		for (auto& child : children) {
			RenderTreeViewRecursive(child);
		}
		ImGui::TreePop();
	}
}

void AssetManager::RenderSelectedFolderContent() {
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
		selectedFolder = selectedFolder->_parent ? selectedFolder->_parent : root;
	}

	ImGui::SameLine();

	ImGui::SetCursorPos({ ImGui::GetCursorPos().x - 1, ImGui::GetCursorPos().y - 1 });
	std::string title(SELECTED_FOLDER_CONTENT_TITLE_TEXT);
	if (selectedFolder != nullptr) {
		title.append(" - ROOT: ");
		title.append(selectedFolder->_path);
	}
	ImGui::PushFont(gui->GetFontBold());
	ImGui::Text(title.c_str());
	ImGui::PopFont();
	if (selectedFolder == nullptr) return;
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
		cmd.append(selectedFolder->_path);
		std::system(cmd.c_str());
	}

	ImGui::NewLine();

	if (ImGui::GetIO().KeyCtrl) {
		float mWheelDelta = ImGui::GetIO().MouseWheel;
		if (selectedFolderContentSettings.thumbnailSize == selectedFolderContentSettings.thumbnailMinSize) {
			if(mWheelDelta < 0) {
				selectedFolderContentSettings.viewMode = mWheelDelta < 0 ? SelectedFolderContentSettings::SelectedFolderContentViewMode::Tree : SelectedFolderContentSettings::SelectedFolderContentViewMode::Column;
			}
		} else {
			selectedFolderContentSettings.thumbnailSize += mWheelDelta;

			selectedFolderContentSettings.thumbnailSize = std::clamp(selectedFolderContentSettings.thumbnailSize, selectedFolderContentSettings.thumbnailMinSize, selectedFolderContentSettings.thumbnailMaxSize);
			selectedFolderContentSettings.itemPadding = selectedFolderContentSettings.thumbnailSize / 4;
		}
	}

	if(selectedFolderContentSettings.viewMode == SelectedFolderContentSettings::SelectedFolderContentViewMode::Column) {
		float cell = selectedFolderContentSettings.thumbnailSize + selectedFolderContentSettings.itemPadding;
		float width = ImGui::GetContentRegionAvail().x;
		selectedFolderContentSettings.minWidth = cell;

		int columns = width / cell;
		if (columns < 1) return;

		bool visible = ImGui::BeginTable("selectedFolder", columns);
		if (!visible) return;

		int i = 0;
		ImGui::TableNextRow();
		for (auto& child : selectedFolder->_children) {
			if (i == columns) {
				i = 0;
				ImGui::TableNextRow();
			}
			ImGui::TableSetColumnIndex(i++);

			void* icon;
			if (child._isDir) {
				icon = gui->GetFolderIcon();
			} else if (Assets::IsSceneFile(child)) {
				icon = gui->GetSceneIcon();
			} else if(child._ext == ".doa") {
				icon = gui->GetProjectIcon();
			} else {
				icon = gui->GetFileIcon();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::ImageButton(icon, { selectedFolderContentSettings.thumbnailSize, selectedFolderContentSettings.thumbnailSize });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				OpenFileAtFileNode(child);
			}
			ImGui::PopStyleColor();

			std::string fileName = child._name;

			float textHeight = ImGui::GetTextLineHeight();
			auto textSize = ImGui::CalcTextSize(fileName.c_str(), nullptr, true, selectedFolderContentSettings.thumbnailSize);
			bool shouldShorten = textSize.y / textHeight > selectedFolderContentSettings.maxTextLine;
			if (shouldShorten) {
				auto begin = fileName.begin();
				auto end = --fileName.end();
				std::string shortened;
				do {
					shortened = std::string(begin, end + 1);
					shortened.append("...");
					textSize = ImGui::CalcTextSize(shortened.c_str(), nullptr, true, selectedFolderContentSettings.thumbnailSize);
					do {
						--end;
					} while (end[0] == ' ');
				} while (textSize.y / textHeight > selectedFolderContentSettings.maxTextLine);
				fileName = shortened;
			}

			ImGui::PushClipRect(ImGui::GetItemRectMin(), { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + (selectedFolderContentSettings.maxTextLine + 0.5f) * textHeight }, true);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + selectedFolderContentSettings.thumbnailSize);
			ImGui::PushFont(gui->GetFontBold());
			ImGui::TextUnformatted(fileName.c_str());
			ImGui::PopFont();
			ImGui::PopTextWrapPos(); ImGui::RenderTextClipped;
			ImGui::PopClipRect();
		}

		ImGui::EndTable();
	}
}

void AssetManager::OpenFileAtFileNode(const FNode& file) {
	FNode* ptr = &const_cast<FNode&>(file);
	if (file._isDir) {
		selectedFolder = ptr;
	} else if(file._isFile) {
		if (Assets::IsSceneFile(file)) {
			gui->openProject->OpenScene(ptr);
		}
	}
}