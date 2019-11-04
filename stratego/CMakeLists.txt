cmake_minimum_required(VERSION 3.13)

if(NOT Torch_FOUND)
	message(STATUS "Torch_FOUND=${Torch_FOUND}")
	if(UNIX)
		set(CMAKE_PREFIX_PATH ../deps/libtorch)
		if(USE_CUDA)
			set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
			set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)
		ENDIF()
	ENDIF(UNIX)
	IF(WIN32)
		IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(CMAKE_PREFIX_PATH deps/libtorch_debug)
		ELSE()
			set(CMAKE_PREFIX_PATH deps/Torch_build/libtorch)
		ENDIF()
	ENDIF(WIN32)

	find_package(Torch REQUIRED)
	# PARENT_SCOPE is necessary to set the torch_found variable of the main cmakelists and not a local only version of it
	set(Torch_FOUND TRUE PARENT_SCOPE)
ENDIF()


set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_SOURCES
		${PROJECT_SOURCE_DIR}/board/PieceStratego.h
		${PROJECT_SOURCE_DIR}/board/BoardStratego.cpp

		#		${PROJECT_SOURCE_DIR}/game/Arena.cpp
		${PROJECT_SOURCE_DIR}/game/Game.h
		${PROJECT_SOURCE_DIR}/game/GameState.h

		${PROJECT_SOURCE_DIR}/nn/model/ActionRepresenter.h
		${PROJECT_SOURCE_DIR}/nn/model/Action.h

		${PROJECT_SOURCE_DIR}/logic/Logic.h

		${PROJECT_SOURCE_DIR}/agent/Agent.h
		${PROJECT_SOURCE_DIR}/agent/AgentReinforce.h
		${PROJECT_SOURCE_DIR}/agent/AgentAlphazero.h

		${PROJECT_SOURCE_DIR}/nn/model/NeuralNet.h
		${PROJECT_SOURCE_DIR}/nn/model/modules/Convolutional.cpp
		${PROJECT_SOURCE_DIR}/nn/model/modules/FullyConnected.cpp
		${PROJECT_SOURCE_DIR}/nn/model/modules/AlphazeroInterface.h
		)


add_library(mylib SHARED
		sources/animation.cpp
		sources/buffers.cpp
		[...]
		)

set(TARGET_SOURCES main.cpp)
# set(TEST_SOURCES  run_tests.cpp test_file1.cpp test_file2.cpp)
include_directories("${PROJECT_SOURCE_DIR}/../deps")
include_directories("${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_executable(AlphaZero++ ${PROJECT_SOURCES} ${TARGET_SOURCES})

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

target_link_libraries(AlphaZero++ "${TORCH_LIBRARIES}")
target_link_libraries(AlphaZero++ "${CUDNN_LIBRARY}")

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(Stratego_CPP stdc++fs)
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})