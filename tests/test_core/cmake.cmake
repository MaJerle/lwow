# CMake include file

# Add more sources
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/test_core.c
)

# Options file
set(LWOW_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/lwow_opts.h)
