cmake_minimum_required(VERSION 3.13)

project(Boarderline)
set(CMAKE_CXX_STANDARD 17)

set (APPLICATION_NAME "Boarderline")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2019-")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")


if(UNIX OR APPLE)
    set(CMAKE_PREFIX_PATH deps/libtorch)
#    set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
#    set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)
ENDIF(UNIX OR APPLE)
IF(WIN32)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_PREFIX_PATH deps/libtorch_debug)
    ELSE()
        set(CMAKE_PREFIX_PATH deps/Torch_build/libtorch)
    ENDIF()
ENDIF(WIN32)

find_package(Torch REQUIRED)

add_subdirectory (boarderline)


# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

enable_testing ()
add_subdirectory (test)
