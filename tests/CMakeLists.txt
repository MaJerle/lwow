cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCXX)

enable_testing()
add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/test_main.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/
)

# Include file that can add more sources and prepare lib parameters
if(DEFINED TEST_CMAKE_FILE_NAME AND NOT "${TEST_CMAKE_FILE_NAME}" STREQUAL "")
    include(${TEST_CMAKE_FILE_NAME})
endif()

# Select OS mutex port matching the host toolchain
if(WIN32)
    set(LWOW_SYS_PORT win32)
else()
    set(LWOW_SYS_PORT pthread)
endif()

# Add subdir with lwow and link to project
add_subdirectory("../lwow" lwow)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwow)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwow_devices)
target_compile_definitions(lwow PUBLIC LWOW_DEV)

if(NOT WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(lwow PUBLIC Threads::Threads)
endif()

# Add test
add_test(NAME Test COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
