//
// Created by michael on 21.10.19.
//

#include "BoardTest.h"
#include "gtest/gtest.h"

using namespace BoardTest;

TEST(BoardTest, BoardTest_BinaryOperator_Tests_Ints) {
    std::map<position_type, kin_type> setup0;
    std::map<position_type, kin_type> setup1;

    setup0[{0, 0}] = {0, 0};
    setup0[{0, 1}] = {3, 0};
    setup0[{0, 2}] = {4, 0};
    setup0[{1, 0}] = {5, 0};
    setup0[{1, 1}] = {3, 1};
    setup0[{1, 2}] = {4, 2};

    kin_type test_kin{5, 1};
    position_type test_pos{3, 2};
    std::shared_ptr<piece_type > test_piece = std::make_shared<piece_type >(test_pos, test_kin, 1);
    setup1[{3, 0}] = {0, 0};
    setup1[{3, 1}] = {5, 0};
    setup1[test_pos] = test_kin;
    setup1[{4, 0}] = {3, 0};
    setup1[{4, 1}] = {3, 2};
    setup1[{4, 2}] = {4, 0};

    std::array<size_t, 2> shape{5, 3};
    std::array<int, 2> starts{0, 0};
    planar_board b{shape, starts, setup0, setup1};

    // check access operator
    EXPECT_EQ(*b[test_pos], *test_piece);

    // check get all pieces of one player method
    std::vector<std::shared_ptr<piece_type> > pieces_0 = b.get_pieces(0);
    for(const auto & piece : pieces_0) {

    }

    // check update board method
    kin_type new_test_kin{5, 1};
    std::shared_ptr<piece_type > new_test_piece = std::make_shared<piece_type >(test_pos, new_test_kin, 1);
    b.update_board(test_pos, new_test_piece);

    EXPECT_EQ(*b[test_pos], *new_test_piece);


    EXPECT_TRUE(m1 == m3);
    EXPECT_TRUE(m1 == m1);
    EXPECT_TRUE(m2 != m3);
    EXPECT_FALSE(m4 == m3);
}