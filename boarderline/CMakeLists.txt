cmake_minimum_required(VERSION 3.13)

project(Boarderline)

set (APPLICATION_NAME "Boarderline")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2019-")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)


if(UNIX)
    set(CMAKE_PREFIX_PATH ../../Torch_build/libtorch)
	set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
	set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)
ENDIF(UNIX)
IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_PREFIX_PATH ../../Torch_build/libtorch_debug)
	ELSE()
		set(CMAKE_PREFIX_PATH ../../Torch_build/libtorch)
	ENDIF()
ENDIF(WIN32)

find_package(Torch REQUIRED)


set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BL_SRCS src/game/GameState.cpp src/game/Game.cpp src/board/Piece.cpp
		src/board/Board.cpp src/game/GameDeclarations.cpp src/logic/StrategoLogic.cpp src/agent/Agent.cpp
		src/utils/RandomSelector.h src/nn/alphazero/MCTS.cpp src/nn/model/NeuralNetwork.cpp ../tqdm/tqdm.h
		src/agent/AgentReinforce.cpp src/nn/training/StateRepresentation.cpp src/nn/training/Coach.cpp src/game/Arena.cpp src/nn/alphazero/ActionRepresentation.cpp src/nn/alphazero/ActionRepresentation.h src/board/Position.h src/board/Move.h)
set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(Stratego_CPP ${COMMON_SOURCES} ${TARGET_SOURCES})

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

target_link_libraries(Stratego_CPP "${TORCH_LIBRARIES}")
#target_link_libraries(Stratego_CPP "${CUDNN_LIBRARY}")
# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
target_link_libraries(Stratego_CPP stdc++fs)
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})