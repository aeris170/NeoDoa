#pragma once

#include <unordered_map>

#include <IconsFontAwesome5Pro.h>

#include <nameof.hpp>

namespace WindowIcons {
	inline constexpr auto SCENE_HIERARCHY_WINDOW_ICON{ ICON_FA_PROJECT_DIAGRAM " " };
	inline constexpr auto OBSERVER_WINDOW_ICON{ ICON_FA_EYE " " };
	inline constexpr auto CONSOLE_WINDOW_ICON{ ICON_FA_CODE " " };
	inline constexpr auto ASSET_MANAGER_WINDOW_ICON{ ICON_FA_CUBES " " };
	inline constexpr auto SCENE_VIEWPORT_WINDOW_ICON{ ICON_FA_DESKTOP " " }; // TODO CHANGE TO ICON_FA_DISPLAY ONCE 6.0 IS AVAILABLE
	inline constexpr auto GAME_VIEWPORT_WINDOW_ICON{ ICON_FA_ALIEN_MONSTER " " };
	inline constexpr auto SCENE_SETTINGS_WINDOW_ICON{ ICON_FA_COG " " };
}

namespace SceneHierarchyIcons {
	inline constexpr auto SCENE_ICON{ ICON_FA_PROJECT_DIAGRAM " " };
	inline constexpr auto ENTITY_ICON{ ICON_FA_CUBE " " };
}

namespace ComponentIcons {
	inline constexpr auto GENERIC_COMPONENT_ICON = ICON_FA_FILE_CODE " "; // or ICON_FA_COG
	inline const std::unordered_map<std::string, const std::string> DEFINED_COMPONENT_ICONS {
		{ nameof(IDComponent), ICON_FA_ADDRESS_CARD " " },
		{ nameof(TransformComponent), ICON_FA_SEEDLING " " },
		{ nameof(ParentComponent), ICON_FA_USER_CROWN " " },
		{ nameof(ChildComponent), ICON_FA_USER " " },
		{ nameof(OrthoCameraComponent), ICON_FA_VIDEO " " },
		{ nameof(PerspectiveCameraComponent), ICON_FA_CAMERA_MOVIE " " },
		{ nameof(ModelRenderer), ICON_FA_COCKTAIL " " }
	};
}

namespace ConsoleIcons {
	inline constexpr auto TRACE_ICON{ ICON_FA_INFO };
	inline constexpr auto INFO_ICON{ ICON_FA_INFO };
	inline constexpr auto WARNING_ICON{ ICON_FA_EXCLAMATION_CIRCLE };
	inline constexpr auto ERROR_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	inline constexpr auto FATAL_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	inline constexpr auto OPENGL_ICON{ ICON_FA_SERVER };
	inline constexpr auto VULKAN_ICON{ ICON_FA_SERVER };
	inline constexpr auto DIRECTX_ICON{ ICON_FA_SERVER };
}

namespace AssetManagerIcons {
	inline constexpr auto REFRESH_BUTTON_ICON{ ICON_FA_SYNC " " };
	inline constexpr auto SELECTED_FOLDER_CONTENT_TITLE_ICON{ ICON_FA_CUBES " " };
}

namespace FileIcons {
	inline constexpr auto DIRECTORY_ICON{ ICON_FA_FOLDER };
}