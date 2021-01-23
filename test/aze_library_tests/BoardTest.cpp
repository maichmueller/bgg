#include "BoardTest.h"

#include "gtest/gtest.h"

using namespace BoardTest;

TEST(BoardTest, BoardTest_Constructor_Ints_2D)
{
   std::map< position_type, token_type > setup0;
   std::map< position_type, token_type > setup1;

   setup0[{0, 0}] = {11, 11};
   setup0[{0, 1}] = {22, 22};
   setup0[{0, 2}] = {33, 33};
   setup0[{1, 0}] = {44, 44};
   setup0[{1, 1}] = {55, 55};
   setup0[{1, 2}] = {66, 66};

   token_type test_kin{20, 1};
   position_type test_pos{3, 2};
   sptr< piece_type > test_piece = std::make_shared< piece_type >(
      test_pos, test_kin, 1);

   setup1[{3, 0}] = {0, 0};
   setup1[{3, 1}] = {5, 0};
   setup1[test_pos] = test_kin;
   setup1[{4, 0}] = {3, 0};
   setup1[{4, 1}] = {3, 2};
   setup1[{4, 2}] = {4, 0};

   std::array< size_t, 2 > shape{5, 3};
   std::array< int, 2 > starts{0, 0};
   planar_board b{shape, starts, setup0, setup1};

   // check access operator
   EXPECT_EQ(*b[test_pos], *test_piece);

   // check get all pieces of one team method
   std::vector< sptr< piece_type > > pieces_0 = b.get_pieces(0);

   // check update board method
   token_type new_test_kin{5, 1};
   sptr< piece_type >
      new_test_piece = std::make_shared< piece_type >(
         test_pos, new_test_kin, 1);
   b.update_board(test_pos, new_test_piece);

   EXPECT_EQ(*b[test_pos], *new_test_piece);
}