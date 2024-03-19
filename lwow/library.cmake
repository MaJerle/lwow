# 
# LIB_PREFIX: LWOW
#
# This file provides set of variables for end user
# and also generates one (or more) libraries, that can be added to the project using target_link_libraries(...)
#
# Before this file is included to the root CMakeLists file (using include() function), user can set some variables:
#
# LWOW_SYS_PORT: If defined, it will include port source file from the library.
# LWOW_OPTS_FILE: If defined, it is the path to the user options file. If not defined, one will be generated for you automatically
# LWOW_COMPILE_OPTIONS: If defined, it provide compiler options for generated library.
# LWOW_COMPILE_DEFINITIONS: If defined, it provides "-D" definitions to the library build
#

# Custom include directory
set(LWOW_CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib_inc)

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
    ${LWOW_CUSTOM_INC_DIR}
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

# Create config file if user didn't provide one info himself
if(NOT LWOW_OPTS_FILE)
    message(STATUS "Using default lwow_opts.h file")
    set(LWOW_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/src/include/lwow/lwow_opts_template.h)
else()
    message(STATUS "Using custom lwow_opts.h file from ${LWOW_OPTS_FILE}")
endif()
configure_file(${LWOW_OPTS_FILE} ${LWOW_CUSTOM_INC_DIR}/lwow_opts.h COPYONLY)

