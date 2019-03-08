cmake_minimum_required(VERSION 3.13)
project(Stratego_CPP)

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES GameState.cpp Game.cpp Piece.cpp Board.cpp GameDeclarations.cpp StrategoLogic.cpp Agent.cpp RandomSelector.h utils.cpp utils.h)
set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)
add_executable(Stratego_CPP ${COMMON_SOURCES} ${TARGET_SOURCES})
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})