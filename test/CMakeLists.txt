

set(TEST_SOURCES
        ${AZE_INCLUDE_DIR}/aze/all.h

        ${STRATEGO_SRC_DIR}/board/PieceStratego.h
        ${STRATEGO_SRC_DIR}/board/BoardStratego.cpp

        ${STRATEGO_SRC_DIR}/game/GameStratego.cpp
        ${STRATEGO_SRC_DIR}/game/StateStratego.cpp

        ${STRATEGO_SRC_DIR}/nn/representation/RepresenterStratego.cpp

        ${STRATEGO_SRC_DIR}/logic/LogicStratego.cpp

        )

set(TARGET_TEST_SOURCES
        main_test.cpp
        aze_library_tests/PositionTest.cpp
        aze_library_tests/MoveTest.cpp
        aze_library_tests/PieceTest.cpp
        aze_library_tests/BoardTest.cpp
        aze_library_tests/StateTest.cpp

        stratego_tests/StateStrategoTest.cpp
        stratego_tests/LogicStrategoTest.cpp
        stratego_tests/RepresenterStrategoTest.cpp
        )

add_executable(tests ${TEST_SOURCES} ${TARGET_TEST_SOURCES})

target_include_directories(tests
        PRIVATE
        ${AZE_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIRS}
        ${TORCH_INCLUDE_DIRS}
        ${STRATEGO_INCLUDE_DIR}
        ${TQDM_INCLUDE_DIR}
        )

set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        )

target_link_libraries(tests aze CONAN_PKG::gtest ${TORCH_LIBRARIES} ${CUDNN_LIBRARY})

add_test(NAME google_testing COMMAND ./tests)
