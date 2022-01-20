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

	float treeViewCurrentWidth = 120;
	float treeViewMinWidth = 30;

	float selectedFolderContentCurrentWidth = 300;
	float selectedFolderContentMinWidth = 120;

	float selectedFolderContentItemPadding = 16;
	float selectedFolderContentThumbnailSize = 64;
	const float selectedFolderContentThumbnailMinSize = 48.0f;
	const float selectedFolderContentThumbnailMaxSize = 96.0f;
	int selectedFolderContentMaxTextLine = 3;

	void RenderMenuBar();
	void RenderTreeView();
	void RenderTreeViewRecursive(FNode* current);

	void RenderSelectedFolderContent();
	void OpenFileAtFileNode(const FNode& file);
};

