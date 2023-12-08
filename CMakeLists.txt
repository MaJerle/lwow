cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwow)
else()
    # Set as executable
    add_executable(${PROJECT_NAME})

    # Add key executable block
    target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.c

        # Port files
        ${CMAKE_CURRENT_LIST_DIR}/lwow/src/system/lwow_sys_win32.c
        ${CMAKE_CURRENT_LIST_DIR}/lwow/src/system/lwow_ll_win32.c
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

    # Compiler options
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    # Add subdir with lwow and link to the project
    set(LWOW_OPTS_DIR ${CMAKE_CURRENT_LIST_DIR}/dev)
    add_subdirectory(lwow)
    target_link_libraries(${PROJECT_NAME} lwow)
    target_link_libraries(${PROJECT_NAME} lwow_devices)
    add_subdirectory(snippets)
    target_link_libraries(${PROJECT_NAME} lwow_snippets)
endif()
