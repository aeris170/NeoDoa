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

find_package(imguizmo CONFIG REQUIRED)

target_link_libraries(Editor PUBLIC imguizmo::imguizmo)
target_link_libraries(Editor PUBLIC ${LUNASVG_LIBRARY})

set(GROUP_LIST
    "main.cpp"
	
    "Attachments/OutlineAttachment.cpp"
    "Attachments/OutlineAttachment.hpp"
	
    "EditorMeta/ComponentInstance.cpp"
    "EditorMeta/ComponentInstance.hpp"
    "EditorMeta/UserDefinedComponentStorage.cpp"
    "EditorMeta/UserDefinedComponentStorage.hpp"
    "EditorMeta/Initializer/EditorMetaInitializer.cpp"
    "EditorMeta/Initializer/EditorMetaInitializer.hpp"
    "EditorMeta/MetaAssetInfo/MetaAssetInfo.cpp"
    "EditorMeta/MetaAssetInfo/MetaAssetInfo.hpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoSerializer.cpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoSerializer.hpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoDeserializer.cpp"
    "EditorMeta/MetaAssetInfo/Serialize/MetaAssetInfoDeserializer.hpp"
    "EditorMeta/Serialize/UserDefinedComponentStorageSerializer.cpp"
    "EditorMeta/Serialize/UserDefinedComponentStorageSerializer.hpp"
    "EditorMeta/Serialize/UserDefinedComponentStorageDeserializer.cpp"
    "EditorMeta/Serialize/UserDefinedComponentStorageDeserializer.hpp"
	
    "Language/CodeGenerator.cpp"
    "Language/CodeGenerator.hpp"
	
    "UI/Colors/Colors.hpp"
    "UI/Components/ComponentUI.cpp"
    "UI/Components/ComponentUI.hpp"
    "UI/Components/ComponentWidgets.cpp"
    "UI/Components/ComponentWidgets.hpp"
    "UI/Extensions/ImGuiExtensions.cpp"
    "UI/Extensions/ImGuiExtensions.hpp"
    "UI/Extensions/FileDialog/FileDialog.cpp"
    "UI/Extensions/FileDialog/FileDialog.hpp"
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
	"UI/GUI/Console/Console.cpp"
	"UI/GUI/Console/Console.hpp"
	"UI/GUI/GameViewport/GameViewport.cpp"
	"UI/GUI/GameViewport/GameViewport.hpp"
	"UI/GUI/MenuBar/MenuBar.cpp"
	"UI/GUI/MenuBar/MenuBar.hpp"
	"UI/GUI/Modals/NewProjectModal.cpp"
	"UI/GUI/Modals/NewProjectModal.hpp"
	"UI/GUI/Observer/Observer.cpp"
	"UI/GUI/Observer/Observer.hpp"
	"UI/GUI/SceneHierarchy/SceneHierarchy.cpp"
	"UI/GUI/SceneHierarchy/SceneHierarchy.hpp"
	"UI/GUI/SceneSettings/SceneSettings.cpp"
	"UI/GUI/SceneSettings/SceneSettings.hpp"
	"UI/GUI/SceneViewport/SceneViewport.cpp"
	"UI/GUI/SceneViewport/SceneViewport.hpp"
	"UI/GUI/SceneViewport/Gizmos/Gizmos.cpp"
	"UI/GUI/SceneViewport/Gizmos/Gizmos.hpp"
	
	"UI/Icons/Icons.hpp"
	"UI/Icons/SVGPathway.cpp"
	"UI/Icons/SVGPathway.hpp"
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
endif()

copy_folder_post_build(Images)
copy_folder_post_build(SVGs)
