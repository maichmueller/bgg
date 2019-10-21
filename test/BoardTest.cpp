//
// Created by michael on 21.10.19.
//

#include "BoardTest.h"
#include "gtest/gtest.h"


TEST(BoardTest, BoardTest_BinaryOperator_Tests_Ints) {
    std::map<position_type, kin_type> setup0;
    std::map<position_type, kin_type> setup1;

    setup0[{0, 0}] = {0, 0};
    setup0[{0, 1}] = {0, 0};
    setup0[{0, 2}] = {0, 0};
    setup0[{1, 0}] = {0, 0};
    setup0[{1, 1}] = {0, 0};
    setup0[{1, 2}] = {0, 0};

    setup1[{0, 0}] = {0, 0};
    setup1[{0, 1}] = {0, 0};
    setup1[{0, 2}] = {0, 0};
    setup1[{1, 0}] = {0, 0};
    setup1[{1, 1}] = {0, 0};
    setup1[{1, 2}] = {0, 0};
    //access operator check
    EXPECT_EQ(m1[0], p1);
    EXPECT_EQ(m1[1], p2);

    EXPECT_TRUE(m1 == m3);
    EXPECT_TRUE(m1 == m1);
    EXPECT_TRUE(m2 != m3);
    EXPECT_FALSE(m4 == m3);
}