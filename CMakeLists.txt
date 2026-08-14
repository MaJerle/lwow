cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwow)
else()
    set(TEST_CMAKE_FILE_NAME "test_core/cmake.cmake")
    add_subdirectory(tests)
endif()
