//
// Created by michael on 21.11.19.
//

#include "StateTest.h"
#include "gtest/gtest.h"


TEST(StateTest, StateTest_Constructor_Ints_2D) {
    size_t board_size = 5;
    std::map<position_type, int > setup0;
    std::map<position_type, int > setup1;

    setup0[{0,0}] = 0;
    setup0[{0,1}] = 1;
    setup0[{0,2}] = 2;
    setup0[{0,3}] = 2;
    setup0[{0,4}] = 3;
    setup0[{1,0}] = 11;
    setup0[{1,1}] = 10;
    setup0[{1,2}] = 2;
    setup0[{1,3}] = 11;
    setup0[{1,4}] = 3;
    setup1[{3,0}] = 2;
    setup1[{3,1}] = 2;
    setup1[{3,2}] = 11;
    setup1[{3,3}] = 2;
    setup1[{3,4}] = 0;
    setup1[{4,0}] = 3;
    setup1[{4,1}] = 1;
    setup1[{4,2}] = 11;
    setup1[{4,3}] = 3;
    setup1[{4,4}] = 10;

    auto state = state_type(std::array<size_t, 2>{5, 5}, std::array<int, 2>{0,0});
}