cmake_minimum_required(VERSION 3.13)

project(AlphaZero++ CXX)
set(CMAKE_CXX_STANDARD 17)

set (APPLICATION_NAME "AlphaZero++")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2019-")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
#
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

set(Torch_FOUND FALSE)
#set(USE_CUDA TRUE)
set(USE_CUDA FALSE)

if(NOT Torch_FOUND)
    message(STATUS "Torch_FOUND=${Torch_FOUND}")
    if(UNIX)
        set(CMAKE_PREFIX_PATH deps/libtorch)
        if(USE_CUDA)
            set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
            set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)
        ENDIF()
    ENDIF(UNIX)
    IF(WIN32)
        IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_PREFIX_PATH deps/libtorch_debug)
        ELSE()
            set(CMAKE_PREFIX_PATH deps/libtorch)
        ENDIF()
    ENDIF(WIN32)

    find_package(Torch REQUIRED)
    # PARENT_SCOPE is necessary to set the torch_found variable of the main cmakelists and not a local only version of it
    set(Torch_FOUND TRUE)
ENDIF()

set(LIBRARY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/azpp")
set(LIBRARY_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

add_subdirectory (include)

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

add_subdirectory(stratego)

add_subdirectory (test)

