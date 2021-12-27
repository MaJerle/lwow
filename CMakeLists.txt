cmake_minimum_required(VERSION 3.0.0)

# Setup project
project(LwLibPROJECT)

# -------------------------------------------------
# This CMakeLists.txt is used only if it is a top-level file.
# Purpose of it is to be able to compile project in standalone way only
#
# When library sources are to be included in another project
# user shall use /lwow/CMakeLists.txt instead
if (NOT PROJECT_IS_TOP_LEVEL)
    message(FATAL_ERROR "This CMakeLists.txt can only be used as top-level. Use /lwow/CMakeLists.txt for library include purpose")
endif()

# Set as executable
add_executable(${PROJECT_NAME})

# Add key executable block
target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/dev/main.c
    )

# Add key include paths
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/dev
    )

# Compilation definition information
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    _DEBUG
    CONSOLE
    LWOW_DEV
    )

# Add subdir with lwow and link to the project
set(LWOW_SYS_ADD_FILES ON)
set(LWOW_SYS_ARCH "win32")
set(LWOW_LL_ADD_FILES ON)
set(LWOW_LL_ARCH "win32")
set(LWOW_DEV_ADD_DS18X20 ON)
add_subdirectory("lwow")
target_link_libraries(${PROJECT_NAME} lwow)
add_subdirectory("snippets")
target_link_libraries(${PROJECT_NAME} lwow_snippets)