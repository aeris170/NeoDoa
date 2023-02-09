#pragma once

#include <unordered_map>

#include <FontAwesome.hpp>

#include <nameof.hpp>

#include "ComponentInstance.hpp"

namespace WindowIcons {
    inline constexpr const char SCENE_HIERARCHY_WINDOW_ICON[]{ ICON_FA_CUBES_STACKED " " };
    inline constexpr const char OBSERVER_WINDOW_ICON[]{ ICON_FA_EYE " " };
    inline constexpr const char CODE_EDITOR_WINDOW_ICON[]{ ICON_FA_FILE_CODE " " };
    inline constexpr const char CONSOLE_WINDOW_ICON[]{ ICON_FA_CODE " " };
    inline constexpr const char ASSET_MANAGER_WINDOW_ICON[]{ ICON_FA_CUBES " " };
    inline constexpr const char SCENE_VIEWPORT_WINDOW_ICON[]{ ICON_FA_DISPLAY " " };
    inline constexpr const char GAME_VIEWPORT_WINDOW_ICON[]{ ICON_FA_ALIEN_8BIT " " };
    inline constexpr const char SCENE_SETTINGS_WINDOW_ICON[]{ ICON_FA_SLIDERS " " };
}

namespace SceneHierarchyIcons {
    inline constexpr const char SCENE_ICON[]{ ICON_FA_CUBES_STACKED " " };
    inline constexpr const char ENTITY_ICON[]{ ICON_FA_CUBE " " };

    namespace ContextMenu {
        inline constexpr const char CREATE_NEW_ENTITY_ICON[]{ ICON_FA_PLUS " " };
        inline constexpr const char CLOSE_SCENE_ICON[]{ ICON_FA_SQUARE_XMARK " " };

        inline constexpr const char ATTACH_COMPONENT_ICON[]{ ICON_FA_LINK " " };
        inline constexpr const char DELETE_ENTITY_ICON[]{ ICON_FA_TRASH " " };
    }
}

namespace SceneViewportIcons {
    namespace Gizmos {
        inline constexpr const char GIZMO_MODE_GLOBAL_ICON[]{ ICON_FA_GLOBE " " };
        inline constexpr const char GIZMO_MODE_LOCAL_ICON[]{ ICON_FA_PERSON " " };
        inline constexpr const char GIZMO_TRANSLATE_ICON[]{ ICON_FA_UP_DOWN_LEFT_RIGHT };
        inline constexpr const char GIZMO_ROTATE_ICON[]{ ICON_FA_ROTATE };
        inline constexpr const char GIZMO_SCALE_ICON[]{ ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER };
    }
    inline constexpr const char PLAY_ICON[]{ ICON_FA_PLAY };
    inline constexpr const char STOP_ICON[]{ ICON_FA_STOP };
}

namespace ComponentIcons {
    inline constexpr const char GENERIC_COMPONENT_ICON[] = ICON_FA_GEAR "  ";
    inline const unordered_string_map<const std::string> DEFINED_COMPONENT_ICONS {
        { nameof(IDComponent), ICON_FA_ID_CARD "  " },
        { nameof(TransformComponent), ICON_FA_SEEDLING "  " },
        { nameof(ParentComponent), ICON_FA_USER_CROWN "  " },
        { nameof(ChildComponent), ICON_FA_USER "  " },
        { nameof(OrthoCameraComponent), ICON_FA_VIDEO "  " },
        { nameof(PerspectiveCameraComponent), ICON_FA_CAMERA_MOVIE "  " },
        { nameof(ModelRenderer), ICON_FA_MARTINI_GLASS_CITRUS "  " }
    };

    inline const std::string& FindIconByComponentName(std::string_view componentName) {
        auto search = DEFINED_COMPONENT_ICONS.find(componentName);
        if (search != ComponentIcons::DEFINED_COMPONENT_ICONS.end()) {
            return search->second;
        } else {
            static std::string gci{ GENERIC_COMPONENT_ICON };
            return gci;
        }
    }

    inline const std::string& FindIconForInstantiationError(InstantiationError error) {
        switch (error) {
            using enum InstantiationError;
            case DEFINITION_MISSING:
            {
                static std::string icon{ ICON_FA_CIRCLE_QUESTION " " };
                return icon;
            }
            case NON_DEFITION_INSTANTIATION:
            {
                static std::string icon{ ICON_FA_OCTAGON_XMARK " " };
                return icon;
            }
            case DEFINITION_COMPILE_ERROR:
            {
                static std::string icon{ ICON_FA_BUG " " };
                return icon;
            }
            case DEFINITION_NOT_DESERIALIZED:
            {
                static std::string icon{ ICON_FA_WRENCH " " };
                return icon;
            }
            default:
            {
                static std::string icon{ ICON_FA_SQUARE_CHECK " " };
                return icon;
            }
        }
    }
}

namespace ObserverIcons {

    namespace ContextMenu {
        inline constexpr const char MOVE_UP_COMPONENT_ICON[]{ ICON_FA_ARROW_UP " " };
        inline constexpr const char MOVE_DOWN_COMPONENT_ICON[]{ ICON_FA_ARROW_DOWN " " };
        inline constexpr const char ATTACH_COMPONENT_ICON[]{ ICON_FA_LINK " " };
        inline constexpr const char RESET_COMPONENT_DATA_ICON[]{ ICON_FA_ARROW_ROTATE_LEFT " " };
        inline constexpr const char DETACH_COMPONENT_ICON[]{ ICON_FA_TRASH " " };
    }
}

namespace ComponentDefinitionViewIcons {
    inline constexpr const char INFO_ICON[]{ ICON_FA_INFO };
    inline constexpr const char WARNING_ICON[]{ ICON_FA_CIRCLE_EXCLAMATION };
    inline constexpr const char ERROR_ICON[]{ ICON_FA_TRIANGLE_EXCLAMATION };
}

namespace ConsoleIcons {
    inline constexpr const char TRACE_ICON[]{ ICON_FA_INFO };
    inline constexpr const char INFO_ICON[]{ ICON_FA_INFO };
    inline constexpr const char WARNING_ICON[]{ ICON_FA_CIRCLE_EXCLAMATION };
    inline constexpr const char ERROR_ICON[]{ ICON_FA_TRIANGLE_EXCLAMATION };
    inline constexpr const char FATAL_ICON[]{ ICON_FA_TRIANGLE_EXCLAMATION };
    inline constexpr const char OPENGL_ICON[]{ ICON_FA_SERVER };
    inline constexpr const char VULKAN_ICON[]{ ICON_FA_SERVER };
    inline constexpr const char DIRECTX_ICON[]{ ICON_FA_SERVER };
}

namespace AssetManagerIcons {
    inline constexpr const char REFRESH_BUTTON_ICON[]{ ICON_FA_ARROWS_ROTATE " " };
    inline constexpr const char SELECTED_FOLDER_CONTENT_TITLE_ICON[]{ ICON_FA_CUBES " " };
}

namespace FileIcons {
    inline constexpr const char DIRECTORY_ICON[]                    { ICON_FA_FOLDER };
    inline constexpr const char DIRECTORY_ICON_MULTIPLE[]           { ICON_FA_FOLDERS };
    inline constexpr const char DIRECTORY_ICON_OPEN[]               { ICON_FA_FOLDER_OPEN };
    inline constexpr const char DIRECTORY_ICON_XMARK[]              { ICON_FA_FOLDER_XMARK };
    inline constexpr const char DIRECTORY_ICON_USER[]               { ICON_FA_FOLDER_USER };
    inline constexpr const char DIRECTORY_ICON_PLUS[]               { ICON_FA_FOLDER_PLUS };
    inline constexpr const char DIRECTORY_ICON_MUSIC[]              { ICON_FA_FOLDER_MUSIC };
    inline constexpr const char DIRECTORY_ICON_MINUS[]              { ICON_FA_FOLDER_MINUS };
    inline constexpr const char DIRECTORY_ICON_MEDICAL[]            { ICON_FA_FOLDER_MEDICAL };
    inline constexpr const char DIRECTORY_ICON_MAGNIFYING_GLASS[]   { ICON_FA_FOLDER_MAGNIFYING_GLASS };
    inline constexpr const char DIRECTORY_ICON_IMAGE[]              { ICON_FA_FOLDER_IMAGE };
    inline constexpr const char DIRECTORY_ICON_HEART[]              { ICON_FA_FOLDER_HEART };
    inline constexpr const char DIRECTORY_ICON_GRID[]               { ICON_FA_FOLDER_GRID };
    inline constexpr const char DIRECTORY_ICON_GEAR[]               { ICON_FA_FOLDER_GEAR };
    inline constexpr const char DIRECTORY_ICON_BOOKMARK[]           { ICON_FA_FOLDER_BOOKMARK };
    inline constexpr const char DIRECTORY_ICON_ARROW_UP[]           { ICON_FA_FOLDER_ARROW_UP };
    inline constexpr const char DIRECTORY_ICON_ARROW_DOWN[]         { ICON_FA_FOLDER_ARROW_DOWN };

    inline const std::array<std::string, 17>& GetAll() {
        static std::array<std::string, 17> icons {
            DIRECTORY_ICON,
            DIRECTORY_ICON_MULTIPLE,
            DIRECTORY_ICON_OPEN,
            DIRECTORY_ICON_XMARK,
            DIRECTORY_ICON_USER,
            DIRECTORY_ICON_PLUS,
            DIRECTORY_ICON_MUSIC,
            DIRECTORY_ICON_MINUS,
            DIRECTORY_ICON_MEDICAL,
            DIRECTORY_ICON_MAGNIFYING_GLASS,
            DIRECTORY_ICON_IMAGE,
            DIRECTORY_ICON_HEART,
            DIRECTORY_ICON_GRID,
            DIRECTORY_ICON_GEAR,
            DIRECTORY_ICON_BOOKMARK,
            DIRECTORY_ICON_ARROW_UP,
            DIRECTORY_ICON_ARROW_DOWN
        };
        return icons;
    }
}