cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWOW: Entering lib source CMakeLists.txt")

# Set library name
set(LWOW_LIB_NAME "lwow")

# Check definitions of some key things
option(LWOW_SYS_ADD_FILES OFF)
if (LWOW_SYS_ADD_FILES)
    message("LWOW: LWOW_SYS_ADD_FILES is set to ON. Looking for LWOW_SYS_ARCH definition")
    if(NOT DEFINED LWOW_SYS_ARCH)
        message("LWOW: LWOW_SYS_ARCH not defined. Using default one: \"win32\"")
        set(LWOW_SYS_ARCH "win32")
    endif()
    message("LWOW: LWOW_SYS_ARCH set to: " ${LWOW_SYS_ARCH})
else()
    message("LWOW: LWOW_SYS_ADD_FILES is set to OFF. User must manually implement system functions in case of operating system support")
endif()

# Low-level file
option(LWOW_LL_ADD_FILES OFF)
if (LWOW_LL_ADD_FILES)
    message("LWOW: LWOW_LL_ADD_FILES is set to ON. Looking for LWOW_LL_ARCH definition")
    if(NOT DEFINED LWOW_LL_ARCH)
        message("LWOW: LWOW_LL_ARCH not defined. Using default one: \"win32\"")
        set(LWOW_LL_ARCH "win32")
    endif()
    message("LWOW: LWOW_LL_ARCH set to: " ${LWOW_LL_ARCH})
else()
    message("LWOW: LWOW_LL_ADD_FILES is set to OFF. User must manually add low-level driver file to project")
endif()

# Register library to the system
add_library(${LWOW_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWOW_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/lwow/lwow.c
    ${CMAKE_CURRENT_LIST_DIR}/src/devices/lwow_device_ds18x20.c
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

# Setup include directories
target_include_directories(${LWOW_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )
