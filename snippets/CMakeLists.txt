cmake_minimum_required(VERSION 3.22)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Snippets sources
set(lwow_snippets_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/scan_devices.c
    )

# Snippets include directories
set(lwow_snippets_include_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/include
    )

# Register library to the system
add_library(lwow_snippets INTERFACE)
target_sources(lwow_snippets PUBLIC ${lwow_snippets_SRCS})
target_include_directories(lwow_snippets INTERFACE ${lwow_snippets_include_DIRS})

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")