cmake_minimum_required(VERSION 3.15)

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
		${LIBRARY_INCLUDE_DIR}/azpp/agent/Agent.h
		${LIBRARY_INCLUDE_DIR}/azpp/agent/AgentReinforce.h
		${LIBRARY_INCLUDE_DIR}/azpp/agent/AgentAlphazero.h

		${LIBRARY_INCLUDE_DIR}/azpp/board/Position.h
		${LIBRARY_INCLUDE_DIR}/azpp/board/Move.h
		${LIBRARY_INCLUDE_DIR}/azpp/board/Piece.h
		${LIBRARY_INCLUDE_DIR}/azpp/board/Board.h

		${LIBRARY_INCLUDE_DIR}/azpp/game/Arena.h
		${LIBRARY_INCLUDE_DIR}/azpp/game/Game.h
		${LIBRARY_INCLUDE_DIR}/azpp/game/State.h

		${LIBRARY_INCLUDE_DIR}/azpp/logic/Logic.h

		${LIBRARY_INCLUDE_DIR}/azpp/mcts/MCTS.h

		${LIBRARY_INCLUDE_DIR}/azpp/nn/model/representation/ActionRepresenter.h
		${LIBRARY_INCLUDE_DIR}/azpp/nn/model/representation/Action.h

		${LIBRARY_INCLUDE_DIR}/azpp/nn/model/NeuralNet.h
		${LIBRARY_INCLUDE_DIR}/azpp/nn/model/modules/AlphazeroInterface.h
		${LIBRARY_INCLUDE_DIR}/azpp/nn/training/Coach.h

		${LIBRARY_INCLUDE_DIR}/azpp/utils/utils.h
		${LIBRARY_INCLUDE_DIR}/azpp/utils/torch_utils.h
		${LIBRARY_INCLUDE_DIR}/azpp/utils/prime_list.h

		${LIBRARY_SOURCE_DIR}/mcts/MCTS.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/NeuralNet.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/Convolutional.cpp
		${LIBRARY_SOURCE_DIR}/nn/model/modules/FullyConnected.cpp


		${LIBRARY_INCLUDE_DIR}/azpp/all.h
		${LIBRARY_INCLUDE_DIR}/azpp/agent.h
		${LIBRARY_INCLUDE_DIR}/azpp/board.h
		${LIBRARY_INCLUDE_DIR}/azpp/game.h
		${LIBRARY_INCLUDE_DIR}/azpp/logic.h
		${LIBRARY_INCLUDE_DIR}/azpp/mcts.h
		${LIBRARY_INCLUDE_DIR}/azpp/nn.h
		${LIBRARY_INCLUDE_DIR}/azpp/utils.h
		)

#include_directories(${LIBRARY_SOURCE_DIR})
#include_directories("${PROJECT_SOURCE_DIR}/deps")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_library(alphazeroPP STATIC ${LIBRARY_SOURCES})

target_include_directories(alphazeroPP
		PUBLIC
		$<BUILD_INTERFACE:${LIBRARY_INCLUDE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
		PRIVATE
		${LIBRARY_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/deps
		)

set_target_properties(alphazeroPP PROPERTIES
		CXX_STANDARD 17
		)

target_compile_features(alphazeroPP PRIVATE cxx_std_17)
# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(alphazeroPP stdc++fs)
target_link_libraries(alphazeroPP "${TORCH_LIBRARIES}")
target_link_libraries(alphazeroPP "${CUDNN_LIBRARY}")
