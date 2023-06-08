cmake_minimum_required(VERSION 3.26.4)

project(Utility LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_LIST
    "constexpr_concat.hpp"
    "defer.hpp"
    "FormatBytes.hpp"
    "nameof.hpp"
    "ObserverPattern.hpp"
    "prettify.hpp"
    "split.hpp"
    "string_map.hpp"
    "TemplateUtilities.hpp"
)

add_library(Utility INTERFACE ${SOURCE_LIST})
target_include_directories(Utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})