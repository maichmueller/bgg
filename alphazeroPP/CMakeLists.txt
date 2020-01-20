cmake_minimum_required(VERSION 3.15)

message(STATUS "TORCH FOUND: ${TORCH_FOUND}")
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
			set(CMAKE_PREFIX_PATH deps/libtorch)
		ENDIF()
	ENDIF(WIN32)

	find_package(Torch REQUIRED)
ENDIF()


set(LIBRARY_SOURCES
		${AZPP_INCLUDE_DIR}/azpp/agent/Agent.h
		${AZPP_INCLUDE_DIR}/azpp/agent/AgentReinforce.h
		${AZPP_INCLUDE_DIR}/azpp/agent/AgentAlphazero.h

		${AZPP_INCLUDE_DIR}/azpp/board/Position.h
		${AZPP_INCLUDE_DIR}/azpp/board/Move.h
		${AZPP_INCLUDE_DIR}/azpp/board/Piece.h
		${AZPP_INCLUDE_DIR}/azpp/board/Board.h

		${AZPP_INCLUDE_DIR}/azpp/game/Arena.h
		${AZPP_INCLUDE_DIR}/azpp/game/Game.h
		${AZPP_INCLUDE_DIR}/azpp/game/State.h

		${AZPP_INCLUDE_DIR}/azpp/logic/Logic.h

		${AZPP_INCLUDE_DIR}/azpp/mcts/MCTS.h

		${AZPP_INCLUDE_DIR}/azpp/nn/representation/Representer.h
		${AZPP_INCLUDE_DIR}/azpp/nn/representation/Action.h

		${AZPP_INCLUDE_DIR}/azpp/nn/model/NeuralNet.h
		${AZPP_INCLUDE_DIR}/azpp/nn/model/modules/AlphazeroInterface.h
		${AZPP_INCLUDE_DIR}/azpp/nn/training/Coach.h

		${AZPP_INCLUDE_DIR}/azpp/utils/utils.h
		${AZPP_INCLUDE_DIR}/azpp/utils/torch_utils.h
		${AZPP_INCLUDE_DIR}/azpp/utils/prime_list.h

		${AZPP_SRC_DIR}/mcts/MCTS.cpp
		${AZPP_SRC_DIR}/nn/model/NeuralNet.cpp
		${AZPP_SRC_DIR}/nn/model/modules/Convolutional.cpp
		${AZPP_SRC_DIR}/nn/model/modules/FullyConnected.cpp


		${AZPP_INCLUDE_DIR}/azpp/all.h
		${AZPP_INCLUDE_DIR}/azpp/azpp.h
		${AZPP_INCLUDE_DIR}/azpp/agent.h
		${AZPP_INCLUDE_DIR}/azpp/board.h
		${AZPP_INCLUDE_DIR}/azpp/game.h
		${AZPP_INCLUDE_DIR}/azpp/logic.h
		${AZPP_INCLUDE_DIR}/azpp/mcts.h
		${AZPP_INCLUDE_DIR}/azpp/nn.h
		${AZPP_INCLUDE_DIR}/azpp/utils.h
		)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_library(alphazeroPP STATIC ${LIBRARY_SOURCES})
target_include_directories(alphazeroPP
		PUBLIC
		${AZPP_INCLUDE_DIR}
		${TQDM_INCLUDE_DIR}
		${TORCH_INCLUDE_DIRS}
		)

set_target_properties(alphazeroPP PROPERTIES
		CXX_STANDARD 17
		)

target_compile_features(alphazeroPP PRIVATE cxx_std_17)
# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
#set(SANITIZE_OPTIONS )
#add_compile_options(${SANITIZE_OPTIONS})
#add_link_options(${SANITIZE_OPTIONS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# add_link_options(--verbose)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(alphazeroPP stdc++fs)
target_link_libraries(alphazeroPP "${TORCH_LIBRARIES}")
target_link_libraries(alphazeroPP "${CUDNN_LIBRARY}")
