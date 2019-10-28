

if(NOT ${Torch_FOUND})
    if(UNIX)
        set(CMAKE_PREFIX_PATH ../deps/libtorch)
        if(USE_CUDA)
            set(CUDNN_LIBRARY /usr/local/cuda/lib64/libcudnn.so)
            set(CUDNN_INCLUDE_DIR /usr/local/cuda/include)
        ENDIF()
        find_package(Torch REQUIRED)
    ENDIF(UNIX)
    IF(WIN32)
        IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_PREFIX_PATH deps/libtorch_debug)
        ELSE()
            set(CMAKE_PREFIX_PATH deps/Torch_build/libtorch)
        ENDIF()

        find_package(Torch REQUIRED)
    ENDIF(WIN32)
ENDIF()

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../azpp")
set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/game/GameState.h
        ${PROJECT_SOURCE_DIR}/board/Piece.h
        ${PROJECT_SOURCE_DIR}/board/Board.h
        ${PROJECT_SOURCE_DIR}/game/GameUtilsStratego.h
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

target_link_libraries(tests stdc++fs gtest_main)
add_test(NAME google_testing COMMAND ./tests)
