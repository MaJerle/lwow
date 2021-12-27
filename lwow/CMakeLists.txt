cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWOW: Entering lib source CMakeLists.txt")

# Set library name
set(LWOW_LIB_NAME "lwow")

# Check definitions of some key things
option(LWOW_SYS_ADD_FILES       "Include system architecture files coming with the library" ON)
option(LWOW_SYS_ARCH            "Architecture to use for file include"                      "win32")
option(LWOW_LL_ADD_FILES        "Include low-level driver files coming with the library"    ON)
option(LWOW_LL_ARCH             "Architecture to use for file include"                      "win32")
option(LWOW_DEV_ADD_DS18X20     "Include DS18x20 device driver to build"                    ON)
message("LWOW: LWOW_SYS_ADD_FILES set to: " ${LWOW_SYS_ADD_FILES})
message("LWOW: LWOW_SYS_ARCH set to: " ${LWOW_SYS_ARCH})
message("LWOW: LWOW_LL_ADD_FILES set to: " ${LWOW_LL_ADD_FILES})
message("LWOW: LWOW_LL_ARCH set to: " ${LWOW_LL_ARCH})
message("LWOW: LWOW_DEV_ADD_DS18X20 set to: " ${LWOW_DEV_ADD_DS18X20})

# Register library to the system
add_library(${LWOW_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWOW_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/lwow/lwow.c
    )

# Setup include directories
target_include_directories(${LWOW_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )

# Add optional sources
if (LWOW_SYS_ADD_FILES)
    target_sources(${LWOW_LIB_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/system/lwow_sys_${LWOW_SYS_ARCH}.c
        )
endif()
if (LWOW_LL_ADD_FILES)
    target_sources(${LWOW_LIB_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/system/lwow_ll_${LWOW_LL_ARCH}.c
        )
endif()
if (LWOW_DEV_ADD_DS18X20)
    target_sources(${LWOW_LIB_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/devices/lwow_device_ds18x20.c
        )
endif()
