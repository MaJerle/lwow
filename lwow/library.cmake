cmake_minimum_required(VERSION 3.22)

# Library core sources
set(lwow_core_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/lwow/lwow.c
)

# Add system port
if(DEFINED LWOW_SYS_PORT)
    set(lwow_core_SRCS
        ${lwow_core_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/system/lwow_sys_${LWOW_SYS_PORT}.c
    )
endif()

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
target_compile_options(lwow PRIVATE ${LWOW_COMPILE_OPTIONS})
target_compile_definitions(lwow PRIVATE ${LWOW_COMPILE_DEFINITIONS})

# Register API to the system
add_library(lwow_devices INTERFACE)
target_sources(lwow_devices PUBLIC ${lwow_devices_SRCS})
target_include_directories(lwow_devices INTERFACE ${lwow_include_DIRS})
target_compile_options(lwow_devices PRIVATE ${LWOW_COMPILE_OPTIONS})
target_compile_definitions(lwow_devices PRIVATE ${LWOW_COMPILE_DEFINITIONS})

# Create config file
if(DEFINED LWOW_OPTS_DIR AND NOT EXISTS ${LWOW_OPTS_DIR}/lwow_opts.h)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/src/include/lwow/lwow_opts_template.h ${LWOW_OPTS_DIR}/lwow_opts.h COPYONLY)
endif()
