cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWOW SNIPPETS: Entering lib source CMakeLists.txt")

# Set library name
set(LWOW_SNIPPETS_LIB_NAME "lwow_snippets")

# Register library to the system
add_library(${LWOW_SNIPPETS_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWOW_SNIPPETS_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/scan_devices.c
    )

# Setup include directories
target_include_directories(${LWOW_SNIPPETS_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
    )
