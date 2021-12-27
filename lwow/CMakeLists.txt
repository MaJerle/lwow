cmake_minimum_required(VERSION 3.13)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Library core sources
set(lwow_core_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/lwow/lwow.c
    )

# Devices
set(lwow_devices_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/devices/lwow_device_ds18x20.c
    )

# Setup include directories
set(lwow_include_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )

# Register core library to the system
add_library(lwow INTERFACE)
target_sources(lwow PUBLIC ${lwow_core_SRCS})
target_include_directories(lwow INTERFACE ${lwow_include_DIRS})

# Register API to the system
add_library(lwow_devices INTERFACE)
target_sources(lwow_devices PUBLIC ${lwow_devices_SRCS})
target_include_directories(lwow_devices INTERFACE ${lwow_include_DIRS})

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
