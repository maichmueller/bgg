

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

set(STRATEGO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stratego")
set(TEST_SOURCES
        ${LIBRARY_SOURCE_DIR}/game/GameState.h
        ${LIBRARY_SOURCE_DIR}/board/Piece.h
        ${LIBRARY_SOURCE_DIR}/board/Board.h
        ${LIBRARY_SOURCE_DIR}/nn/model/representation/ActionRepresenter.h
        ${LIBRARY_SOURCE_DIR}/board/Position.h
        ${LIBRARY_SOURCE_DIR}/board/Move.h
        ${LIBRARY_SOURCE_DIR}/nn/model/representation/Action.h

        ${STRATEGO_SOURCE_DIR}/board/PieceStratego.h
        ${STRATEGO_SOURCE_DIR}/game/GameStateStratego.h
        ${STRATEGO_SOURCE_DIR}/board/BoardStratego.cpp
        ${STRATEGO_SOURCE_DIR}/nn/model/representation/ActionRepresenterStratego.cpp)


set(TARGET_TEST_SOURCES
        main_test.cpp
        PositionTest.cpp
        MoveTest.cpp
        BoardTest.cpp)

add_executable(tests ${TEST_SOURCES} ${TARGET_TEST_SOURCES})
include_directories("${PROJECT_SOURCE_DIR}/azpp")
target_link_libraries(tests stdc++fs gtest_main)
add_test(NAME google_testing COMMAND ./tests)
