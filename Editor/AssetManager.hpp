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

	float treeViewMinWidth = 30;
	float selectedFolderContentMinWidth = 128;
	float selectedFolderContentItemPadding = 16;
	float selectedFolderContentThumbnailSize = 64;
	int selectedFolderContentMaxTextLine = 3;

	void RenderMenuBar();
	void RenderTreeView();
	void RenderTreeViewRecursive(FNode* current);

	void RenderSelectedFolderContent();
	void OpenFileAtFileNode(const FNode& file);
};
