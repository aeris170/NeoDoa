#pragma once

#include <unordered_map>

#include <IconsFontAwesome5Pro.h>

#include <nameof.hpp>

namespace WindowIcons {
	inline constexpr const char* const SCENE_HIERARCHY_WINDOW_ICON{ ICON_FA_PROJECT_DIAGRAM " "};
	inline constexpr const char* const OBSERVER_WINDOW_ICON{ ICON_FA_EYE " "};
	inline constexpr const char* const CONSOLE_WINDOW_ICON{ ICON_FA_CODE " "};
	inline constexpr const char* const ASSET_MANAGER_WINDOW_ICON{ ICON_FA_CUBES " "};
}

namespace SceneHierarchyIcons {
	inline constexpr const char* const SCENE_ICON = ICON_FA_PROJECT_DIAGRAM " ";
	inline constexpr const char* const ENTITY_ICON = ICON_FA_CUBE " ";
}

namespace ComponentIcons {
	inline constexpr const char* const GENERIC_COMPONENT_ICON = ICON_FA_FILE_CODE " "; // or ICON_FA_COG
	inline const std::unordered_map<std::string, const std::string> DEFINED_COMPONENT_ICONS{
		{ nameof(IDComponent), ICON_FA_ADDRESS_CARD " " },
		{ nameof(TransformComponent), ICON_FA_SEEDLING " " },
		{ nameof(ParentComponent), ICON_FA_USER_CROWN " " },
		{ nameof(ChildComponent), ICON_FA_USER " " },
		{ nameof(Camera), ICON_FA_VIDEO " " },
		{ nameof(ModelRenderer), ICON_FA_COCKTAIL " " }
	};
}

namespace ConsoleIcons {
	inline constexpr const char* const TRACE_ICON{ ICON_FA_INFO };
	inline constexpr const char* const INFO_ICON{ ICON_FA_INFO };
	inline constexpr const char* const WARNING_ICON{ ICON_FA_EXCLAMATION_CIRCLE };
	inline constexpr const char* const ERROR_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	inline constexpr const char* const FATAL_ICON{ ICON_FA_EXCLAMATION_TRIANGLE };
	inline constexpr const char* const OPENGL_ICON{ ICON_FA_SERVER };
	inline constexpr const char* const VULKAN_ICON{ ICON_FA_SERVER };
	inline constexpr const char* const DIRECTX_ICON{ ICON_FA_SERVER };
}

namespace AssetManagerIcons {
	inline constexpr const char* const REFRESH_BUTTON_ICON{ ICON_FA_SYNC " " };
	inline constexpr const char* const SELECTED_FOLDER_CONTENT_TITLE_ICON{ ICON_FA_CUBES " " };
}