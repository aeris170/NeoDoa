#pragma once

#include <IconsFontAwesome5Pro.h>

struct GUI;
struct Assets;
struct FNode;

struct AssetManager {

	static constexpr const char* const REFRESH_BUTTON_TEXT{ ICON_FA_SYNC " Refresh" };
	static constexpr const char* const SELECTED_FOLDER_CONTENT_TITLE_TEXT{ ICON_FA_CUBES " Assets" };

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

	float treeViewCurrentWidth = 120.0f;
	float treeViewMinWidth = 30.0f;

	float selectedFolderContentCurrentWidth = 300.0f;
	float selectedFolderContentMinWidth = 120.0f;

	float selectedFolderContentItemPadding = 16.0f;
	float selectedFolderContentThumbnailSize = 64.0f;
	const float selectedFolderContentThumbnailMinSize = 48.0f;
	const float selectedFolderContentThumbnailMaxSize = 96.0f;
	int selectedFolderContentMaxTextLine = 3;

	void RenderMenuBar();
	void RenderTreeView();
	void RenderTreeViewRecursive(FNode* current);

	void RenderSelectedFolderContent();
	void OpenFileAtFileNode(const FNode& file);
};

