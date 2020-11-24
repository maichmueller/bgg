cmake_minimum_required(VERSION 3.15)

project(AZE++ CXX)
#set(CMAKE_CXX_STANDARD 17)

set(APPLICATION_NAME "AlphaZero++")
set(APPLICATION_CODENAME "${PROJECT_NAME}")
set(APPLICATION_COPYRIGHT_YEARS "2019-")


option(USE_CUDA "Use CUDA for Neural Network Processing" ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(APPLE)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
endif()
message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif()
endif()

# Later on it might become interesting to add a gui to the project
set(AZE_USE_IMGUI FALSE)

set(CMAKE_CONFIG_FOLDER "cmake")

# Set this path if the conan install is not in the standard directory
# This doesn't work due to a conan internal error when conan is not found in the standard directory (/usr/bin on linux)
# It doesn't work in general when CONAN_COMMAND is set it seems.
#execute_process(COMMAND which conan OUTPUT_VARIABLE CONAN_PATH)


# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(${CMAKE_CONFIG_FOLDER}/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which
    # is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    target_precompile_headers(project_options
            INTERFACE <vector> <string> <map> <utility> <memory> <array> <list> <stack>)
endif()

# Set up some extra Conan dependencies based on our needs
# before loading Conan
set(CONAN_EXTRA_REQUIRES "")
set(CONAN_EXTRA_OPTIONS "")

if(AZE_USE_IMGUI)
    set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
            imgui-sfml/2.1@bincrafters/stable)

    # set(CONAN_EXTRA_OPTIONS ${CONAN_EXTRA_OPTIONS} sfml:shared=False
    # sfml:graphics=True sfml:audio=False sfml:window=True
    # libalsa:disable_python=True)
endif()

include(cmake/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)

message("CMAKE_PREFIX_PATH option: ${CMAKE_PREFIX_PATH}")
find_package(Torch)
if (NOT TORCH_FOUND)
    if (UNIX AND NOT "${CMAKE_PREFIX_PATH}")
        set(CMAKE_PREFIX_PATH deps/libtorch)
    ENDIF ()
    IF (WIN32 AND NOT "${CMAKE_PREFIX_PATH}")
        IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_PREFIX_PATH deps/libtorch_debug)
        ELSE ()
            set(CMAKE_PREFIX_PATH deps/libtorch)
        ENDIF ()
    ENDIF ()

    find_package(Torch REQUIRED)
ENDIF ()

if (TORCH_FOUND)
    message(STATUS "Found Torch: \nInclude: ${TORCH_INCLUDE_DIRS}; \nLibrary: ${TORCH_LIBRARIES}")
endif (TORCH_FOUND)
enable_testing()


set(TQDM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/tqdm/include")

set(AZE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/aze/cppsrc")
set(AZE_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/aze/include)
add_library(aze STATIC)
add_subdirectory(aze)

set(STRATEGO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stratego")
set(STRATEGO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stratego")
add_subdirectory(stratego)

add_subdirectory(test)

