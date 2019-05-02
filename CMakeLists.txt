cmake_minimum_required(VERSION 3.13)
project(Stratego_CPP)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/libtorch/include)

if(UNIX)
    set(CMAKE_PREFIX_PATH /home/michael/GitHub/Stratego_CPP/libtorch)
ENDIF(UNIX)
IF(WIN32)
    set(CMAKE_PREFIX_PATH C:/Users/Michael/Documents/GitHub/Stratego_CPP/libtorch)
ENDIF(WIN32)
find_package(Torch REQUIRED)

set(COMMON_SOURCES tqdm_utils.cpp GameState.cpp Game.cpp Piece.cpp
                   Board.cpp GameDeclarations.cpp StrategoLogic.cpp Agent.cpp
                   RandomSelector.h MCTS.cpp NeuralNetwork.cpp tqdm/tqdm.h
                   AgentReinforce.cpp torch_utils.cpp)
set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)
add_executable(Stratego_CPP ${COMMON_SOURCES} ${TARGET_SOURCES})

target_link_libraries(Stratego_CPP "${TORCH_LIBRARIES}")
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})