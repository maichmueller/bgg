cmake_minimum_required(VERSION 3.13)

#if(UNIX)
#	set(CMAKE_PREFIX_PATH ../deps/libtorch)
	#    set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
	#    set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)

#	find_package(Torch REQUIRED)
#ENDIF(UNIX)
#IF(WIN32)
#	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
#		set(CMAKE_PREFIX_PATH deps/libtorch_debug)
#	ELSE()
#		set(CMAKE_PREFIX_PATH deps/Torch_build/libtorch)
#	ENDIF()
#
#	find_package(Torch REQUIRED)
#ENDIF(WIN32)


set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_SOURCES
		${PROJECT_SOURCE_DIR}/board/Position.h
		${PROJECT_SOURCE_DIR}/board/Move.h
		${PROJECT_SOURCE_DIR}/board/Piece.h
		${PROJECT_SOURCE_DIR}/board/Board.h
		${PROJECT_SOURCE_DIR}/game/GameState.h
		${PROJECT_SOURCE_DIR}/nn/model/ActionRepresenter.h
		${PROJECT_SOURCE_DIR}/game/Game.h
		#		${PROJECT_SOURCE_DIR}/game/GameUtilsStratego.h
		#		${PROJECT_SOURCE_DIR}/logic/StrategoLogic.cpp
		#		${PROJECT_SOURCE_DIR}/agent/Agent.cpp
		#		${PROJECT_SOURCE_DIR}/utils/RandomSelector.h
		#		${PROJECT_SOURCE_DIR}/nn/alphazero/MCTS.cpp
		#		${PROJECT_SOURCE_DIR}/nn/model/NeuralNet.cpp
		#		${PROJECT_SOURCE_DIR}/agent/AgentReinforce.cpp
		#		${PROJECT_SOURCE_DIR}/nn/training/StateRepresentation.cpp
		#		${PROJECT_SOURCE_DIR}/nn/training/Coach.cpp
		#		${PROJECT_SOURCE_DIR}/game/Arena.cpp
#		${PROJECT_SOURCE_DIR}/utils/UniversallyUniqueId.h
#
		${PROJECT_SOURCE_DIR}/board/PieceStratego.h
		${PROJECT_SOURCE_DIR}/board/BoardStratego.cpp
		${PROJECT_SOURCE_DIR}/game/GameStateStratego.h
		${PROJECT_SOURCE_DIR}/game/GameStratego.h
#		${PROJECT_SOURCE_DIR}/nn/model/Action.h
#		${PROJECT_SOURCE_DIR}/nn/model/ActionRepresenterStratego.cpp
		)


set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)
include_directories("${PROJECT_SOURCE_DIR}/../deps")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_executable(Stratego_CPP ${PROJECT_SOURCES} ${TARGET_SOURCES})

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

target_link_libraries(Stratego_CPP "${TORCH_LIBRARIES}")
target_link_libraries(Stratego_CPP "${CUDNN_LIBRARY}")

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(Stratego_CPP stdc++fs)
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})