cmake_minimum_required(VERSION 3.26.4)

project(Launcher LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Launcher)

target_link_libraries(Launcher PUBLIC Engine)

set(GROUP_LIST
    "main.cpp"
	
	"GUI.cpp"
	"GUI.hpp"
	
	"ProjectData/ProjectData.cpp"
	"ProjectData/ProjectData.hpp"
	
	"FileDialog.cpp"
	"FileDialog.hpp"
	
	"Modals/ErrorModal.cpp"
	"Modals/ErrorModal.hpp"
	"Modals/ImportProjectModal.cpp"
	"Modals/ImportProjectModal.hpp"
	"Modals/NewProjectModal.cpp"
	"Modals/NewProjectModal.hpp"
)

foreach(source IN LISTS GROUP_LIST)
    get_filename_component(source_path "${source}" PATH)
    get_filename_component(source_name "${source}" NAME)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source_name}")
    target_sources(Launcher PRIVATE "${source_name}")
endforeach()

if(MSVC)
 target_compile_options(Launcher PRIVATE "/MP")
 target_link_options(Launcher PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
 target_compile_options(Launcher PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()

copy_folder_post_build(Images)