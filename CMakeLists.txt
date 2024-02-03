cmake_minimum_required(VERSION 3.26.4)

function(copy_folder_post_build folder)
  add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/${folder}
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${folder})
endfunction()

if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_option($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_option($<$<CONFIG:Debug>:/ZI>)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(NeoDoa)

add_compile_definitions(ASSIMP_BUILD_ZLIB=ON)
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_subdirectory(Submodules/angelscript_addons_impl)
add_subdirectory(Submodules/detector)
add_subdirectory(Submodules/EZEasing)
add_subdirectory(Submodules/IconFontCppHeaders)
add_subdirectory(Submodules/imgInspect)
add_subdirectory(Submodules/stb_impl)
add_subdirectory(Utility)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Launcher)

set_target_properties(angelscript_addons_impl PROPERTIES FOLDER Submodules)
set_target_properties(detector PROPERTIES FOLDER Submodules)
set_target_properties(EZEasing PROPERTIES FOLDER Submodules)
set_target_properties(IconFontCppHeaders PROPERTIES FOLDER Submodules)
set_target_properties(imgInspect PROPERTIES FOLDER Submodules)
set_target_properties(stb_impl PROPERTIES FOLDER Submodules)

set_target_properties(Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
set_target_properties(Launcher PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)