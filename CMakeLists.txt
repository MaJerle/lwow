cmake_minimum_required(VERSION 3.0.0)
project(LwLibPROJECT VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    lwow/src/lwow/lwow.c
    lwow/src/system/lwow_ll_win32.c
    lwow/src/system/lwow_sys_win32.c
    lwow/src/devices/lwow_device_ds18x20.c
    snippets/scan_devices.c
    dev/VisualStudio/main.c
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    dev/VisualStudio
    lwow/src/include
    snippets/include
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    _DEBUG
    CONSOLE
    TEST_CONSOLE_123
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
