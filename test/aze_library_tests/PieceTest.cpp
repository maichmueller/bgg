

#include "PieceTest.h"

#include "gtest/gtest.h"

TEST(PositionTest, KinTest)
{
   kin_type k{3, 0};
   kin_type k1{-3, 11};
   kin_type k2{-3, -4};

   EXPECT_EQ(k.length(), 2);
   EXPECT_EQ(k[0], 3);
   EXPECT_EQ(k[1], 0);

   EXPECT_EQ(k1.length(), 2);
   EXPECT_EQ(k1[0], -3);
   EXPECT_EQ(k1[1], 11);

   EXPECT_EQ(k2.length(), 2);
   EXPECT_EQ(k2[0], -3);
   EXPECT_EQ(k2[1], -4);

   size_t index = 0;
   for(auto val : k) {
      EXPECT_EQ(val, k[index]);
      ++index;
   }

   EXPECT_TRUE(k == k);
   EXPECT_FALSE(k == k1);
   EXPECT_FALSE(k == k2);
}

TEST(PositionTest, PieceTest)
{
   piece_type p({3, 0}, {-4, 0}, 1);
   piece_type p1({10, 3}, {1, 1}, 0);
   piece_type p2({0, 1}, {-4, 1}, 1);

   EXPECT_EQ(p.get_role(), kin_type(-4, 0));
   EXPECT_EQ(p.get_position(), position_type(3, 0));

   EXPECT_EQ(p1.get_role(), kin_type(1, 1));
   EXPECT_EQ(p1.get_position(), position_type(10, 3));

   EXPECT_EQ(p2.get_role(), kin_type(-4, 1));
   EXPECT_EQ(p2.get_position(), position_type(0, 1));
}