

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
            set(CMAKE_PREFIX_PATH ../deps/libtorch_debug)
        ELSE()
            set(CMAKE_PREFIX_PATH ../deps/libtorch)
        ENDIF()
    ENDIF(WIN32)

    find_package(Torch REQUIRED)
ENDIF()

set(STRATEGO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stratego")

set(TEST_SOURCES
        ${AZPP_INCLUDE_DIR}/azpp/all.h

        ${STRATEGO_SOURCE_DIR}/board/PieceStratego.h
        ${STRATEGO_SOURCE_DIR}/board/BoardStratego.cpp

        ${STRATEGO_SOURCE_DIR}/game/GameStratego.cpp
        ${STRATEGO_SOURCE_DIR}/game/StateStratego.cpp

        ${STRATEGO_SOURCE_DIR}/nn/model/representation/RepresenterStratego.cpp

        ${STRATEGO_SOURCE_DIR}/logic/LogicStratego.cpp

        ${STRATEGO_SOURCE_DIR}/nn/model/modules/AlphazeroStratego.h

        )

set(TARGET_TEST_SOURCES
        azpp_library_tests/main_test.cpp
        azpp_library_tests/PositionTest.cpp
        azpp_library_tests/MoveTest.cpp
        azpp_library_tests/PieceTest.cpp
        azpp_library_tests/BoardTest.cpp
        azpp_library_tests/StateTest.cpp

        stratego_tests/StateStrategoTest.cpp
        stratego_tests/LogicStrategoTest.cpp
        stratego_tests/RepresenterStrategoTest.cpp
        )

add_executable(tests ${TEST_SOURCES} ${TARGET_TEST_SOURCES})

target_include_directories(tests
        PRIVATE
        ${AZPP_INCLUDE_DIR}
        ${STRATEGO_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/deps
        ${EXEC_SOURCE_DIR}
        )

set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        )

target_link_libraries(tests gtest_main)
target_link_libraries(tests "${AZPP_LIBRARY}")
target_link_libraries(tests "${TORCH_LIBRARIES}")
target_link_libraries(tests "${CUDNN_LIBRARY}")

add_test(NAME google_testing COMMAND ./tests)
