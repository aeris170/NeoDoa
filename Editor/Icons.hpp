#pragma once

#include <utility>
#include <unordered_map>

#include <Utility/nameof.hpp>
#include <Utility/string_map.hpp>

#include <Engine/FontAwesome.hpp>

#include <Editor/ComponentInstance.hpp>

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
    using KeyType = const char*;
    using ValueType = std::string;
    using ElementType = std::pair<KeyType, ValueType>;

    inline constexpr const char DIRECTORY_ICON_OPEN[]             { ICON_FA_FOLDER_OPEN             };
    inline constexpr const char DIRECTORY_ICON[]                  { ICON_FA_FOLDER                  };
    inline constexpr const char DIRECTORY_ICON_MULTIPLE[]         { ICON_FA_FOLDERS                 };
    inline constexpr const char DIRECTORY_ICON_XMARK[]            { ICON_FA_FOLDER_XMARK            };
    inline constexpr const char DIRECTORY_ICON_USER[]             { ICON_FA_FOLDER_USER             };
    inline constexpr const char DIRECTORY_ICON_PLUS[]             { ICON_FA_FOLDER_PLUS             };
    inline constexpr const char DIRECTORY_ICON_MUSIC[]            { ICON_FA_FOLDER_MUSIC            };
    inline constexpr const char DIRECTORY_ICON_MINUS[]            { ICON_FA_FOLDER_MINUS            };
    inline constexpr const char DIRECTORY_ICON_MEDICAL[]          { ICON_FA_FOLDER_MEDICAL          };
    inline constexpr const char DIRECTORY_ICON_MAGNIFYING_GLASS[] { ICON_FA_FOLDER_MAGNIFYING_GLASS };
    inline constexpr const char DIRECTORY_ICON_IMAGE[]            { ICON_FA_FOLDER_IMAGE            };
    inline constexpr const char DIRECTORY_ICON_HEART[]            { ICON_FA_FOLDER_HEART            };
    inline constexpr const char DIRECTORY_ICON_GRID[]             { ICON_FA_FOLDER_GRID             };
    inline constexpr const char DIRECTORY_ICON_GEAR[]             { ICON_FA_FOLDER_GEAR             };
    inline constexpr const char DIRECTORY_ICON_BOOKMARK[]         { ICON_FA_FOLDER_BOOKMARK         };
    inline constexpr const char DIRECTORY_ICON_ARROW_UP[]         { ICON_FA_FOLDER_ARROW_UP         };
    inline constexpr const char DIRECTORY_ICON_ARROW_DOWN[]       { ICON_FA_FOLDER_ARROW_DOWN       };
    inline std::array<ElementType, 17> DirectoryIcons {
        std::pair{ DIRECTORY_ICON_OPEN,             "folder-open"             },
        std::pair{ DIRECTORY_ICON,                  "folder"                  },
        std::pair{ DIRECTORY_ICON_MULTIPLE,         "folders"                 },
        std::pair{ DIRECTORY_ICON_XMARK,            "folder-xmark"            },
        std::pair{ DIRECTORY_ICON_USER,             "folder-user"             },
        std::pair{ DIRECTORY_ICON_PLUS,             "folder-plus"             },
        std::pair{ DIRECTORY_ICON_MUSIC,            "folder-music"            },
        std::pair{ DIRECTORY_ICON_MINUS,            "folder-minus"            },
        std::pair{ DIRECTORY_ICON_MEDICAL,          "folder-medical"          },
        std::pair{ DIRECTORY_ICON_MAGNIFYING_GLASS, "folder-magnifying-glass" },
        std::pair{ DIRECTORY_ICON_IMAGE,            "folder-image"            },
        std::pair{ DIRECTORY_ICON_HEART,            "folder-heart"            },
        std::pair{ DIRECTORY_ICON_GRID,             "folder-grid"             },
        std::pair{ DIRECTORY_ICON_GEAR,             "folder-gear"             },
        std::pair{ DIRECTORY_ICON_BOOKMARK,         "folder-bookmark"         },
        std::pair{ DIRECTORY_ICON_ARROW_UP,         "folder-arrow-up"         },
        std::pair{ DIRECTORY_ICON_ARROW_DOWN,       "folder-arrow-down"       },
    };

    inline constexpr const char SCENE_ICON[]{ ICON_FA_CUBES_STACKED };
    inline std::array<ElementType, 1> SceneIcons{
        std::pair{ SCENE_ICON, "cubes-stacked" }
    };

    inline constexpr const char TEXTURE_ICON[]{ ICON_FA_IMAGE };
    inline std::array<ElementType, 1> TextureIcons{
        std::pair{ TEXTURE_ICON, "image" }
    };

    inline constexpr const char COMPONENT_ICON[]{ ICON_FA_GEAR };
    inline std::array<ElementType, 1> ComponentIcons{
        std::pair{ COMPONENT_ICON, "gear" }
    };

    inline constexpr const char SHADER_ICON[]           { ICON_FA_CIRCLE_S };
    inline constexpr const char VERTEX_SHADER_ICON[]    { ICON_FA_CIRCLE_V };
    inline constexpr const char TESS_CTRL_SHADER_ICON[] { ICON_FA_CIRCLE_T };
    inline constexpr const char TESS_EVAL_SHADER_ICON[] { ICON_FA_SQUARE_T };
    inline constexpr const char GEOMETRY_SHADER_ICON[]  { ICON_FA_CIRCLE_G };
    inline constexpr const char FRAGMENT_SHADER_ICON[]  { ICON_FA_CIRCLE_F };
    inline constexpr const char COMPUTE_SHADER_ICON[]   { ICON_FA_CIRCLE_C };
    inline std::array<ElementType, 7> ShaderIcons{
        std::pair{ SHADER_ICON,           "circle-s" },
        std::pair{ VERTEX_SHADER_ICON,    "circle-v" },
        std::pair{ TESS_CTRL_SHADER_ICON, "circle-t" },
        std::pair{ TESS_EVAL_SHADER_ICON, "square-t" },
        std::pair{ GEOMETRY_SHADER_ICON,  "circle-g" },
        std::pair{ FRAGMENT_SHADER_ICON,  "circle-f" },
        std::pair{ COMPUTE_SHADER_ICON,   "circle-c" }
    };
    inline constexpr const char SHADER_PROGRAM_ICON[]{ ICON_FA_TRIANGLE };
    inline std::array<ElementType, 1> ShaderProgramIcons{
        std::pair{ SHADER_PROGRAM_ICON, "triangle" }
    };


    inline constexpr const char FILE_ICON[]                    { ICON_FA_FILE                    };
    inline constexpr const char FILE_ICON_MULTIPLE[]           { ICON_FA_FILES                   };
    inline constexpr const char FILE_ICON_ARROW_DOWN[]         { ICON_FA_FILE_ARROW_DOWN         };
    inline constexpr const char FILE_ICON_ARROW_UP[]           { ICON_FA_FILE_ARROW_UP           };
    inline constexpr const char FILE_ICON_AUDIO[]              { ICON_FA_FILE_AUDIO              };
    inline constexpr const char FILE_ICON_BINARY[]             { ICON_FA_FILE_BINARY             };
    inline constexpr const char FILE_ICON_CERTIFICATE[]        { ICON_FA_FILE_CERTIFICATE        };
    inline constexpr const char FILE_ICON_COLUMN[]             { ICON_FA_FILE_CHART_COLUMN       };
    inline constexpr const char FILE_ICON_CHART_PIE[]          { ICON_FA_FILE_CHART_PIE          };
    inline constexpr const char FILE_ICON_CHECK[]              { ICON_FA_FILE_CHECK              };
    inline constexpr const char FILE_ICON_CIRCLE_CHECK[]       { ICON_FA_FILE_CIRCLE_CHECK       };
    inline constexpr const char FILE_ICON_CIRCLE_EXCLAMATION[] { ICON_FA_FILE_CIRCLE_EXCLAMATION };
    inline constexpr const char FILE_ICON_CIRCLE_INFO[]        { ICON_FA_FILE_CIRCLE_INFO        };
    inline constexpr const char FILE_ICON_CIRCLE_MINUS[]       { ICON_FA_FILE_CIRCLE_MINUS       };
    inline constexpr const char FILE_ICON_CIRCLE_PLUS[]        { ICON_FA_FILE_CIRCLE_PLUS        };
    inline constexpr const char FILE_ICON_CIRCLE_QUESTION[]    { ICON_FA_FILE_CIRCLE_QUESTION    };
    inline constexpr const char FILE_ICON_CIRCLE_XMARK[]       { ICON_FA_FILE_CIRCLE_XMARK       };
    inline constexpr const char FILE_ICON_CODE[]               { ICON_FA_FILE_CODE               };
    inline constexpr const char FILE_ICON_CONTRACT[]           { ICON_FA_FILE_CONTRACT           };
    inline constexpr const char FILE_ICON_CSV[]                { ICON_FA_FILE_CSV                };
    inline constexpr const char FILE_ICON_DASHED_LINE[]        { ICON_FA_FILE_DASHED_LINE        };
    inline constexpr const char FILE_ICON_EXCEL[]              { ICON_FA_FILE_EXCEL              };
    inline constexpr const char FILE_ICON_EXCLAMATION[]        { ICON_FA_FILE_EXCLAMATION        };
    inline constexpr const char FILE_ICON_EXPORT[]             { ICON_FA_FILE_EXPORT             };
    inline constexpr const char FILE_ICON_HEART[]              { ICON_FA_FILE_HEART              };
    inline constexpr const char FILE_ICON_IMAGE[]              { ICON_FA_FILE_IMAGE              };
    inline constexpr const char FILE_ICON_IMPORT[]             { ICON_FA_FILE_IMPORT             };
    inline constexpr const char FILE_ICON_INVOICE[]            { ICON_FA_FILE_INVOICE            };
    inline constexpr const char FILE_ICON_INVOICE_DOLLAR[]     { ICON_FA_FILE_INVOICE_DOLLAR     };
    inline constexpr const char FILE_ICON_LINES[]              { ICON_FA_FILE_LINES              };
    inline constexpr const char FILE_ICON_LOCK[]               { ICON_FA_FILE_LOCK               };
    inline constexpr const char FILE_ICON_MAGNIFYING_GLASS[]   { ICON_FA_FILE_MAGNIFYING_GLASS   };
    inline constexpr const char FILE_ICON_MEDICAL[]            { ICON_FA_FILE_MEDICAL            };
    inline constexpr const char FILE_ICON_MEDICAL_MULTIPLE[]   { ICON_FA_FILES_MEDICAL           };
    inline constexpr const char FILE_ICON_MINUS[]              { ICON_FA_FILE_MINUS              };
    inline constexpr const char FILE_ICON_MUSIC[]              { ICON_FA_FILE_MUSIC              };
    inline constexpr const char FILE_ICON_PDF[]                { ICON_FA_FILE_PDF                };
    inline constexpr const char FILE_ICON_PEN[]                { ICON_FA_FILE_PEN                };
    inline constexpr const char FILE_ICON_PLUS[]               { ICON_FA_FILE_PLUS               };
    inline constexpr const char FILE_ICON_PLUS_MINUS[]         { ICON_FA_FILE_PLUS_MINUS         };
    inline constexpr const char FILE_ICON_POWERPOINT[]         { ICON_FA_FILE_POWERPOINT         };
    inline constexpr const char FILE_ICON_PRESCRIPTION[]       { ICON_FA_FILE_PRESCRIPTION       };
    inline constexpr const char FILE_ICON_SHIELD[]             { ICON_FA_FILE_SHIELD             };
    inline constexpr const char FILE_ICON_SIGNATURE[]          { ICON_FA_FILE_SIGNATURE          };
    inline constexpr const char FILE_ICON_SLASH[]              { ICON_FA_FILE_SLASH              };
    inline constexpr const char FILE_ICON_SPREADSHEET[]        { ICON_FA_FILE_SPREADSHEET        };
    inline constexpr const char FILE_ICON_USER[]               { ICON_FA_FILE_USER               };
    inline constexpr const char FILE_ICON_VIDEO[]              { ICON_FA_FILE_VIDEO              };
    inline constexpr const char FILE_ICON_WAVEFORM[]           { ICON_FA_FILE_WAVEFORM           };
    inline constexpr const char FILE_ICON_WORD[]               { ICON_FA_FILE_WORD               };
    inline constexpr const char FILE_ICON_XMARK[]              { ICON_FA_FILE_XMARK              };
    inline constexpr const char FILE_ICON_ZIPPER[]             { ICON_FA_FILE_ZIPPER             };
    inline std::array<ElementType, 52> RegularFileIcons{
        std::pair{ FILE_ICON,                    "file"                    },
        std::pair{ FILE_ICON_MULTIPLE,           "files"                   },
        std::pair{ FILE_ICON_ARROW_DOWN,         "file-arrow-down"         },
        std::pair{ FILE_ICON_ARROW_UP,           "file-arrow-up"           },
        std::pair{ FILE_ICON_AUDIO,              "file-audio"              },
        std::pair{ FILE_ICON_BINARY,             "file-binary"             },
        std::pair{ FILE_ICON_CERTIFICATE,        "file-certificate"        },
        std::pair{ FILE_ICON_COLUMN,             "file-chart-column"       },
        std::pair{ FILE_ICON_CHART_PIE,          "file-chart-pie"          },
        std::pair{ FILE_ICON_CHECK,              "file-check"              },
        std::pair{ FILE_ICON_CIRCLE_CHECK,       "file-circle-check"       },
        std::pair{ FILE_ICON_CIRCLE_EXCLAMATION, "file-circle-exclamation" },
        std::pair{ FILE_ICON_CIRCLE_INFO,        "file-circle-info"        },
        std::pair{ FILE_ICON_CIRCLE_MINUS,       "file-circle-minus"       },
        std::pair{ FILE_ICON_CIRCLE_PLUS,        "file-circle-plus"        },
        std::pair{ FILE_ICON_CIRCLE_QUESTION,    "file-circle-question"    },
        std::pair{ FILE_ICON_CIRCLE_XMARK,       "file-circle-xmark"       },
        std::pair{ FILE_ICON_CODE,               "file-code"               },
        std::pair{ FILE_ICON_CONTRACT,           "file-contract"           },
        std::pair{ FILE_ICON_CSV,                "file-csv"                },
        std::pair{ FILE_ICON_DASHED_LINE,        "file-dashed-line"        },
        std::pair{ FILE_ICON_EXCEL,              "file-excel"              },
        std::pair{ FILE_ICON_EXCLAMATION,        "file-exclamation"        },
        std::pair{ FILE_ICON_EXPORT,             "file-export"             },
        std::pair{ FILE_ICON_HEART,              "file-heart"              },
        std::pair{ FILE_ICON_IMAGE,              "file-image"              },
        std::pair{ FILE_ICON_IMPORT,             "file-import"             },
        std::pair{ FILE_ICON_INVOICE,            "file-invoice"            },
        std::pair{ FILE_ICON_INVOICE_DOLLAR,     "file-invoice-dollar"     },
        std::pair{ FILE_ICON_LINES,              "file-lines"              },
        std::pair{ FILE_ICON_LOCK,               "file-lock"               },
        std::pair{ FILE_ICON_MAGNIFYING_GLASS,   "file-magnifying-glass"   },
        std::pair{ FILE_ICON_MEDICAL,            "file-medical"            },
        std::pair{ FILE_ICON_MEDICAL_MULTIPLE,   "files-medical"           },
        std::pair{ FILE_ICON_MINUS,              "file-minus"              },
        std::pair{ FILE_ICON_MUSIC,              "file-music"              },
        std::pair{ FILE_ICON_PDF,                "file-pdf"                },
        std::pair{ FILE_ICON_PEN,                "file-pen"                },
        std::pair{ FILE_ICON_PLUS,               "file-plus"               },
        std::pair{ FILE_ICON_PLUS_MINUS,         "file-plus-minus"         },
        std::pair{ FILE_ICON_POWERPOINT,         "file-powerpoint"         },
        std::pair{ FILE_ICON_PRESCRIPTION,       "file-prescription"       },
        std::pair{ FILE_ICON_SHIELD,             "file-shield"             },
        std::pair{ FILE_ICON_SIGNATURE,          "file-signature"          },
        std::pair{ FILE_ICON_SLASH,              "file-slash"              },
        std::pair{ FILE_ICON_SPREADSHEET,        "file-spreadsheet"        },
        std::pair{ FILE_ICON_USER,               "file-user"               },
        std::pair{ FILE_ICON_VIDEO,              "file-video"              },
        std::pair{ FILE_ICON_WAVEFORM,           "file-waveform"           },
        std::pair{ FILE_ICON_WORD,               "file-word"               },
        std::pair{ FILE_ICON_XMARK,              "file-xmark"              },
        std::pair{ FILE_ICON_ZIPPER,             "file-zipper"             }
    };
}
