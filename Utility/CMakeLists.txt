cmake_minimum_required(VERSION 3.26.4)

project(Utility LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Utility STATIC)
target_include_directories(Utility PUBLIC ..)

find_package(EnTT CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc dt i18n)

target_link_libraries(Utility PUBLIC EnTT::EnTT)
target_link_libraries(Utility PUBLIC ICU::uc ICU::dt ICU::i18n)

set(GROUP_LIST
    "CheckSubstring.cpp"
    "CheckSubstring.hpp"
    "ConstexprConcat.hpp"
    "FormatBytes.cpp"
    "FormatBytes.hpp"
    "NameOf.cpp"
    "NameOf.hpp"
    "ObserverPattern.cpp"
    "ObserverPattern.hpp"
    "Prettify.cpp"
    "Prettify.hpp"
	"Split.cpp"
    "Split.hpp"
	"StringMap.cpp"
    "StringMap.hpp"
    "TemplateUtilities.hpp"
	"Trim.cpp"
    "Trim.hpp"
	"UndoRedoStack.cpp"
	"UndoRedoStack.hpp"
)

foreach(source IN LISTS GROUP_LIST)
    get_filename_component(source_path "${source}" PATH)
    get_filename_component(source_name "${source}" NAME)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source_name}")
    target_sources(Utility PRIVATE "${source_name}")
endforeach()


if(MSVC)
 target_compile_options(Utility PRIVATE "/MP")
endif()