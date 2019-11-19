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
	# PARENT_SCOPE is necessary to set the torch_found variable of the main cmakelists
	# and not a local only version of it
	set(Torch_FOUND TRUE PARENT_SCOPE)
ENDIF()


set(STRATEGO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stratego")
set(STRATEGO_SOURCES
		${STRATEGO_SOURCE_DIR}/board/PieceStratego.h
		${STRATEGO_SOURCE_DIR}/board/BoardStratego.cpp

		#		${PROJECT_STRATEGO_SOURCE_DIR}/game/Arena.cpp
		${STRATEGO_SOURCE_DIR}/game/GameStratego.h
		${STRATEGO_SOURCE_DIR}/game/StateStratego.cpp

		${STRATEGO_SOURCE_DIR}/nn/model/representation/ActionRepresenterStratego.cpp

		${STRATEGO_SOURCE_DIR}/logic/LogicStratego.cpp

		${STRATEGO_SOURCE_DIR}/nn/model/modules/AlphazeroStratego.h
		)


set(TARGET_SOURCES main.cpp)

add_executable(alphazero_stratego ${STRATEGO_SOURCES} ${TARGET_SOURCES})

target_include_directories(alphazero_stratego
		PRIVATE
		${LIBRARY_INCLUDE_DIR}
		${PROJECT_SOURCE_DIR}/deps
		${STRATEGO_SOURCE_DIR}
		)

set_target_properties(alphazero_stratego PROPERTIES
		CXX_STANDARD 17
		)

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
target_compile_options(alphazero_stratego PRIVATE -Wall ${SANITIZE_OPTIONS})
target_link_options(alphazero_stratego PRIVATE ${SANITIZE_OPTIONS} --verbose)

target_link_libraries(alphazero_stratego "${alphazeroPP}")
target_link_libraries(alphazero_stratego "${TORCH_LIBRARIES}")
target_link_libraries(alphazero_stratego "${CUDNN_LIBRARY}")
# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(alphazero_stratego stdc++fs)