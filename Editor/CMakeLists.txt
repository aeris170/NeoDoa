cmake_minimum_required(VERSION 3.26.4)

project(Editor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Editor)

target_link_libraries(Editor PUBLIC EZEasing)
target_link_libraries(Editor PUBLIC imgInspect)
target_link_libraries(Editor PUBLIC Engine)

find_path(LUNASVG_INCLUDE_DIR lunasvg.h)
target_include_directories(Editor PRIVATE ${LUNASVG_INCLUDE_DIR})
find_library(LUNASVG_LIBRARY lunasvg)

find_package(argparse CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)

target_link_libraries(Editor PUBLIC argparse::argparse)
target_link_libraries(Editor PUBLIC imguizmo::imguizmo)
target_link_libraries(Editor PUBLIC cppzmq cppzmq-static)
target_link_libraries(Editor PUBLIC ${LUNASVG_LIBRARY})

set(GROUP_LIST
    "main.cpp"
	
    "Attachments/OutlineAttachment.cpp"
    "Attachments/OutlineAttachment.hpp"

    "EditorMeta/EditorMeta.cpp"
    "EditorMeta/EditorMeta.hpp"
    "EditorMeta/MetaAssetInfo/MetaAssetInfo.cpp"
    "EditorMeta/MetaAssetInfo/MetaAssetInfo.hpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoSerializer.cpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoSerializer.hpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoDeserializer.cpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoDeserializer.hpp"
    "EditorMeta/Scripting/ComponentInstance.cpp"
    "EditorMeta/Scripting/ComponentInstance.hpp"
    "EditorMeta/Scripting/UserDefinedComponentStorage.cpp"
    "EditorMeta/Scripting/UserDefinedComponentStorage.hpp"
    "EditorMeta/Scripting/Serialize/UserDefinedComponentStorageSerializer.cpp"
    "EditorMeta/Scripting/Serialize/UserDefinedComponentStorageSerializer.hpp"
    "EditorMeta/Scripting/Serialize/UserDefinedComponentStorageDeserializer.cpp"
    "EditorMeta/Scripting/Serialize/UserDefinedComponentStorageDeserializer.hpp"
	
    "Language/CodeGenerator.cpp"
    "Language/CodeGenerator.hpp"
	
    "UI/Colors/Colors.hpp"
    "UI/Components/ComponentUI.cpp"
    "UI/Components/ComponentUI.hpp"
    "UI/Components/ComponentWidgets.cpp"
    "UI/Components/ComponentWidgets.hpp"
    "UI/Extensions/ImGuiExtensions.cpp"
    "UI/Extensions/ImGuiExtensions.hpp"
    "UI/Extensions/Shortcuts/KeyboardShortcutHandler.cpp"
    "UI/Extensions/Shortcuts/KeyboardShortcutHandler.hpp"
    "UI/Extensions/TextEditor/TextEditor.cpp"
    "UI/Extensions/TextEditor/TextEditor.hpp"
    "UI/Extensions/TextEditor/LanguageDefinitions.cpp"
    "UI/Extensions/TextEditor/ImGuiDebugPanel.cpp"
	
    "UI/GUI/GUI.cpp"
    "UI/GUI/GUI.hpp"
    "UI/GUI/AssetManager/AssetManager.cpp"
    "UI/GUI/AssetManager/AssetManager.hpp"
    "UI/GUI/CodeEditor/CodeEditor.cpp"
    "UI/GUI/CodeEditor/CodeEditor.hpp"
    "UI/GUI/Commands/GUICommand.cpp"
    "UI/GUI/Commands/GUICommand.hpp"
    "UI/GUI/Commands/AddEntityCommand.cpp"
    "UI/GUI/Commands/AddEntityCommand.hpp"
    "UI/GUI/Commands/RemoveEntityCommand.cpp"
    "UI/GUI/Commands/RemoveEntityCommand.hpp"
    "UI/GUI/Commands/RenameEntityCommand.cpp"
    "UI/GUI/Commands/RenameEntityCommand.hpp"
    "UI/GUI/Commands/RotateEntityCommand.cpp"
    "UI/GUI/Commands/RotateEntityCommand.hpp"
    "UI/GUI/Commands/ScaleEntityCommand.cpp"
    "UI/GUI/Commands/ScaleEntityCommand.hpp"
    "UI/GUI/Commands/TranslateEntityCommand.cpp"
    "UI/GUI/Commands/TranslateEntityCommand.hpp"
    "UI/GUI/Console/Console.cpp"
    "UI/GUI/Console/Console.hpp"
    "UI/GUI/GameViewport/GameViewport.cpp"
    "UI/GUI/GameViewport/GameViewport.hpp"
    "UI/GUI/MenuBar/MenuBar.cpp"
    "UI/GUI/MenuBar/MenuBar.hpp"
    "UI/GUI/MenuBar/UndoRedoHistory.cpp"
    "UI/GUI/MenuBar/UndoRedoHistory.hpp"
    "UI/GUI/Modals/NewAssetModal.cpp"
    "UI/GUI/Modals/NewAssetModal.hpp"
    "UI/GUI/Observer/Observer.cpp"
    "UI/GUI/Observer/Observer.hpp"
    "UI/GUI/Observer/Displays/DisplayTargetRenderer.cpp"
    "UI/GUI/Observer/Displays/DisplayTargetRenderer.hpp"
    "UI/GUI/Observer/Displays/SceneDisplay/SceneDisplay.cpp"
    "UI/GUI/Observer/Displays/SceneDisplay/SceneDisplay.hpp"
    "UI/GUI/Observer/Displays/ComponentDefinitionDisplay/ComponentDefinitionDisplay.cpp"
    "UI/GUI/Observer/Displays/ComponentDefinitionDisplay/ComponentDefinitionDisplay.hpp"
    "UI/GUI/Observer/Displays/ShaderDisplay/ShaderDisplay.cpp"
    "UI/GUI/Observer/Displays/ShaderDisplay/ShaderDisplay.hpp"
    "UI/GUI/Observer/Displays/ShaderProgramDisplay/ShaderProgramDisplay.cpp"
    "UI/GUI/Observer/Displays/ShaderProgramDisplay/ShaderProgramDisplay.hpp"
    "UI/GUI/Observer/Displays/MaterialDisplay/MaterialDisplay.cpp"
    "UI/GUI/Observer/Displays/MaterialDisplay/MaterialDisplay.hpp"
    "UI/GUI/SceneHierarchy/SceneHierarchy.cpp"
    "UI/GUI/SceneHierarchy/SceneHierarchy.hpp"
    "UI/GUI/SceneSettings/SceneSettings.cpp"
    "UI/GUI/SceneSettings/SceneSettings.hpp"
    "UI/GUI/SceneViewport/SceneViewport.cpp"
    "UI/GUI/SceneViewport/SceneViewport.hpp"
    "UI/GUI/SceneViewport/SceneViewportCameraSettings.cpp"
    "UI/GUI/SceneViewport/SceneViewportCameraSettings.hpp"
    "UI/GUI/SceneViewport/Gizmos/Gizmos.cpp"
    "UI/GUI/SceneViewport/Gizmos/Gizmos.hpp"
	
    "UI/Icons/Icons.hpp"
    "UI/Icons/SVGPathway.cpp"
    "UI/Icons/SVGPathway.hpp"
	
    "UI/Strings/Strings.hpp"
)

foreach(source IN LISTS GROUP_LIST)
    get_filename_component(source_path "${source}" PATH)
    get_filename_component(source_name "${source}" NAME)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source_name}")
    target_sources(Editor PRIVATE "${source_name}")
endforeach()

if(MSVC)
 target_compile_options(Editor PRIVATE "/MP")
 target_link_options(Editor PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
 target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()

copy_folder_post_build(Images)
copy_folder_post_build(SVGs)
