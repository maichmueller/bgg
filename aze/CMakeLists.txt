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
		${AZE_SRC_DIR}/mcts/MCTS.cpp
		${AZE_SRC_DIR}/nn/model/NeuralNet.cpp
		${AZE_SRC_DIR}/nn/model/modules/Convolutional.cpp
		${AZE_SRC_DIR}/nn/model/modules/FullyConnected.cpp
		)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(aze PRIVATE ${LIBRARY_SOURCES})

target_include_directories(aze
		PRIVATE
		${AZE_INCLUDE_DIR}
		${TQDM_INCLUDE_DIR}
		${TORCH_INCLUDE_DIRS}
		)
target_include_directories(aze
		SYSTEM INTERFACE
		${AZE_INCLUDE_DIR}
		${TQDM_INCLUDE_DIR}
		${TORCH_INCLUDE_DIRS}
		)

set_target_properties(aze PROPERTIES
		CXX_STANDARD 17
		)

target_compile_features(aze PRIVATE cxx_std_17)
# set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined)
#set(SANITIZE_OPTIONS )
#add_compile_options(${SANITIZE_OPTIONS})
#add_link_options(${SANITIZE_OPTIONS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# add_link_options(--verbose)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} LESS 9)
	target_link_libraries(aze stdc++fs)
endif()
target_link_libraries(aze "${TORCH_LIBRARIES}")
target_link_libraries(aze "${CUDNN_LIBRARY}")
