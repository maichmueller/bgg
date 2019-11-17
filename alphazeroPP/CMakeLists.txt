cmake_minimum_required(VERSION 3.13)

if(NOT TORCH_FOUND)
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


set(LIBRARY_SOURCES
		${LIBRARY_SOURCE_DIR}/agent/Agent.h
		${LIBRARY_SOURCE_DIR}/agent/AgentReinforce.h
		${LIBRARY_SOURCE_DIR}/agent/AgentAlphazero.h

		${LIBRARY_SOURCE_DIR}/board/Position.h
		${LIBRARY_SOURCE_DIR}/board/Move.h
		${LIBRARY_SOURCE_DIR}/board/Piece.h
		${LIBRARY_SOURCE_DIR}/board/Board.h

		${LIBRARY_SOURCE_DIR}/game/Arena.h
		${LIBRARY_SOURCE_DIR}/game/Game.h
		${LIBRARY_SOURCE_DIR}/game/State.h

		${LIBRARY_SOURCE_DIR}/logic/Logic.h

		${LIBRARY_SOURCE_DIR}/mcts/MCTS.cpp
		${LIBRARY_SOURCE_DIR}/mcts/MCTS.h

		${LIBRARY_SOURCE_DIR}/nn/model/representation/ActionRepresenter.h
		${LIBRARY_SOURCE_DIR}/nn/model/representation/Action.h

		${LIBRARY_SOURCE_DIR}/nn/model/NeuralNet.h
		${LIBRARY_SOURCE_DIR}/nn/model/modules/Convolutional.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/FullyConnected.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/AlphazeroInterface.h
		${LIBRARY_SOURCE_DIR}/nn/training/Coach.h

		${LIBRARY_SOURCE_DIR}/utils/utils.h
		${LIBRARY_SOURCE_DIR}/utils/torch_utils.h
		${LIBRARY_SOURCE_DIR}/utils/prime_list.h

		${LIBRARY_SOURCE_DIR}/../include/azpp/all.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/agent.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/board.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/game.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/logic.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/mcts.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/nn.h
		${LIBRARY_SOURCE_DIR}/../include/azpp/utils.h
		)

#include_directories(${LIBRARY_SOURCE_DIR})
#include_directories("${PROJECT_SOURCE_DIR}/deps")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_library(alphazeroPP STATIC ${LIBRARY_SOURCES})

target_include_directories(alphazeroPP
		PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${LIBRARY_INCLUDE_DIR}>
		PRIVATE
		${LIBRARY_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/deps
		)

set_target_properties(alphazeroPP PROPERTIES
		CXX_STANDARD 17
		)

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
target_link_libraries(alphazeroPP stdc++fs)
target_link_libraries(alphazeroPP "${TORCH_LIBRARIES}")
target_link_libraries(alphazeroPP "${CUDNN_LIBRARY}")
