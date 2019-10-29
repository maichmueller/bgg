

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

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../azpp")
set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/game/GameState.h
        ${PROJECT_SOURCE_DIR}/board/Piece.h
        ${PROJECT_SOURCE_DIR}/board/Board.h
        ${PROJECT_SOURCE_DIR}/nn/model/ActionRepresenter.h
        ${PROJECT_SOURCE_DIR}/board/Position.h
        ${PROJECT_SOURCE_DIR}/board/Move.h

        ${PROJECT_SOURCE_DIR}/board/PieceStratego.h
        ${PROJECT_SOURCE_DIR}/game/GameStateStratego.h
        ${PROJECT_SOURCE_DIR}/board/BoardStratego.cpp
        ${PROJECT_SOURCE_DIR}/nn/model/Action.h
        ${PROJECT_SOURCE_DIR}/nn/model/ActionRepresenterStratego.cpp)


set(TARGET_TEST_SOURCES
        main_test.cpp
        PositionTest.cpp
        MoveTest.cpp
        BoardTest.cpp)

add_executable(tests ${TEST_SOURCES} ${TARGET_TEST_SOURCES})
include_directories("${CMAKE_SOURCE_DIR}/azpp")
target_link_libraries(tests stdc++fs gtest_main)
add_test(NAME google_testing COMMAND ./tests)
