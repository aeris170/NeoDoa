#pragma once

#include <IconsFontAwesome5Pro.h>

struct GUI;
struct Assets;
struct FNode;

struct AssetManager {

	static constexpr auto REFRESH_BUTTON_TEXT{ "Refresh" };
	static constexpr auto SELECTED_FOLDER_CONTENT_TITLE_TEXT{ "Assets" };

	GUI* const gui;

	AssetManager(GUI* gui) noexcept;

	void Begin();
	void Render();
	void End();

private:
	bool hasContent{ false };
	Assets* assets{ nullptr };
	FNode* root{ nullptr };
	FNode* selectedFolder{ nullptr };

	struct TreeViewSettings {
		float currentWidth = 120.0f;
		float minWidth = 30.0f;
	} treeViewSettings;

	struct SelectedFolderContentSettings {
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
	} selectedFolderContentSettings;

	void RenderMenuBar();
	void RenderTreeView();
	void RenderTreeViewRecursive(FNode* current);

	void RenderSelectedFolderContent();
	void OpenFileAtFileNode(const FNode& file);
};

