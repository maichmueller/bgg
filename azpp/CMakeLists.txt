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


set(LIBRARY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/azpp")
set(LIBRARY_SOURCES
		${LIBRARY_SOURCE_DIR}/board/Position.h
		${LIBRARY_SOURCE_DIR}/board/Move.h
		${LIBRARY_SOURCE_DIR}/board/Piece.h
		${LIBRARY_SOURCE_DIR}/board/Board.h

		${LIBRARY_SOURCE_DIR}/utils/utils.h
		${LIBRARY_SOURCE_DIR}/utils/prime_list.h

		#		${PROJECT_SOURCE_DIR}/game/Arena.cpp
		${LIBRARY_SOURCE_DIR}/game/Game.h
		${LIBRARY_SOURCE_DIR}/game/GameState.h

		${LIBRARY_SOURCE_DIR}/nn/model/representation/ActionRepresenter.h
		${LIBRARY_SOURCE_DIR}/nn/model/representation/Action.h

		${LIBRARY_SOURCE_DIR}/logic/Logic.h

		${LIBRARY_SOURCE_DIR}/agent/Agent.h
		${LIBRARY_SOURCE_DIR}/agent/AgentReinforce.h
		${LIBRARY_SOURCE_DIR}/agent/AgentAlphazero.h

		${LIBRARY_SOURCE_DIR}/nn/model/NeuralNet.h
		${LIBRARY_SOURCE_DIR}/nn/model/modules/Convolutional.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/FullyConnected.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/AlphazeroInterface.h
		)

include_directories("${LIBRARY_SOURCE_DIR}/../deps")
include_directories("${LIBRARY_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_library(alphazeroPP STATIC ${LIBRARY_SOURCES})


# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(alphazero++ stdc++fs)
target_link_libraries(alphazeroPP "${TORCH_LIBRARIES}")
target_link_libraries(alphazeroPP "${CUDNN_LIBRARY}")

