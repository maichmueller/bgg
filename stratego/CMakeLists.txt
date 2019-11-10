cmake_minimum_required(VERSION 3.13)

if(NOT Torch_FOUND)
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


set(EXEC_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stratego")
set(PROJECT_SOURCES
		${EXEC_SOURCE_DIR}/board/PieceStratego.h
		${EXEC_SOURCE_DIR}/board/BoardStratego.cpp

		#		${PROJECT_EXEC_SOURCE_DIR}/game/Arena.cpp
		${EXEC_SOURCE_DIR}/game/GameStratego.h
		${EXEC_SOURCE_DIR}/game/GameStrategoState.h

		${EXEC_SOURCE_DIR}/nn/model/ActionRepresenterStratego.h

		${EXEC_SOURCE_DIR}/logic/LogicStratego.h

		${EXEC_SOURCE_DIR}/nn/model/modules/AlphazeroStratego.h
		)


set(TARGET_SOURCES main.cpp)
include_directories("${CMAKE_SOURCE_DIR}/deps")
include_directories("${EXEC_SOURCE_DIR}")
include_directories("${LIBRARY_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_executable(alphazero_stratego ${EXEC_SOURCE_DIR} ${TARGET_SOURCES})

# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
set(SANITIZE_OPTIONS )
add_compile_options(${SANITIZE_OPTIONS})
add_link_options(${SANITIZE_OPTIONS})
# add_link_options(--verbose)

target_link_libraries(alphazero_stratego alphazeroPP)
target_link_libraries(alphazero_stratego "${TORCH_LIBRARIES}")
target_link_libraries(alphazero_stratego "${CUDNN_LIBRARY}")

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
#target_link_libraries(Stratego_CPP stdc++fs)
# add_executable(tests ${COMMON_SOURCES} ${TEST_SOURCES})