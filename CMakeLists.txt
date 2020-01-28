cmake_minimum_required(VERSION 3.15)

project(AlphaZero++ CXX)
#set(CMAKE_CXX_STANDARD 17)

set (APPLICATION_NAME "AlphaZero++")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2019-")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

set(USE_CUDA TRUE)
#set(USE_CUDA FALSE)

if(NOT TORCH_FOUND)
    if(UNIX)
        set(CMAKE_PREFIX_PATH deps/libtorch)
#        if(USE_CUDA)
#            set(CUDNN_INCLUDE_PATH /usr/local/cuda/include)
#            set(CUDNN_LIBRARY_PATH /usr/local/cuda/lib64/libcudnn.so)
#        ENDIF()
    ENDIF(UNIX)
    IF(WIN32)
        IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_PREFIX_PATH deps/libtorch_debug)
        ELSE()
            set(CMAKE_PREFIX_PATH deps/libtorch)
        ENDIF()
    ENDIF(WIN32)

    find_package(Torch REQUIRED)
ENDIF()

if(TORCH_FOUND)
    message(STATUS "Found Torch: \nInclude: ${TORCH_INCLUDE_DIRS}; \nLibrary: ${TORCH_LIBRARIES}")
endif(TORCH_FOUND)
enable_testing ()

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

set(TQDM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/tqdm/include")
message(STATUS "TQDM found: ${TQDM_INCLUDE_DIR}")


set(AZPP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/alphazeroPP/cppsrc")
set(AZPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/alphazeroPP/include")
add_subdirectory (alphazeroPP)
set(AZPP_LIBRARY ${CMAKE_SOURCE_DIR}/build/debug/alphazeroPP/libalphazeroPP.a)

set(STRATEGO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stratego")
add_subdirectory(stratego)

add_subdirectory (test)

