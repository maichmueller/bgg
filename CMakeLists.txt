cmake_minimum_required(VERSION 3.13)
project(Stratego_CPP)

set(CMAKE_CXX_STANDARD 17)


if(UNIX)
    # set(CMAKE_PREFIX_PATH /home/michael/GitHub/Torch_build/libtorch)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Torch_build/libtorch)
ENDIF(UNIX)
IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_PREFIX_PATH C:/Users/Michael/Documents/GitHub/Torch_build/libtorch_win_debug)
	ELSE()
		set(CMAKE_PREFIX_PATH C:/Users/Michael/Documents/GitHub/Torch_build/libtorch_win)
	ENDIF()
ENDIF(WIN32)
find_package(Torch REQUIRED)

#include_directories(${CMAKE_SOURCE_DIR}/libtorch/include)

set(COMMON_SOURCES GameState.cpp Game.cpp Piece.cpp
                   Board.cpp GameDeclarations.cpp StrategoLogic.cpp Agent.cpp
                   RandomSelector.h MCTS.cpp NeuralNetwork.cpp tqdm/tqdm.h
                   AgentReinforce.cpp StateRepresentation.cpp Coach.cpp Arena.cpp)
set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)
add_executable(Stratego_CPP ${COMMON_SOURCES} ${TARGET_SOURCES})

target_link_libraries(Stratego_CPP "${TORCH_LIBRARIES}")
# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
target_link_libraries(Stratego_CPP stdc++fs)
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})