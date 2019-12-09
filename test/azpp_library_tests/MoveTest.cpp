#include <cmath>
#include "MoveTest.h"
#include "gtest/gtest.h"
#include "PositionTest.h"


TEST(MoveTest, MoveTest_BinaryOperator_Tests_Ints) {
constexpr size_t dim = 4;
    int_pos<dim> p1{4, 6, 7, 2};
    int_pos<dim> p2{4, -2, 7, 2};
    int_move<dim> m1{p1, p2};
    int_move<dim> m3{p1, p2};

    int_pos<dim> p3{1, 0, 0, 1};
    int_pos<dim> p4{1, 1, 1, 1};
    int_move<dim> m2{p3, p4};

    int_pos<dim> p5{-1, -2, 0, 1};
    int_pos<dim> p6{0, 0, 3, -1};
    int_move<dim> m4{p5, p6};

    //access operator check
    EXPECT_EQ(m1[0], p1);
    EXPECT_EQ(m1[1], p2);

    EXPECT_TRUE(m1 == m3);
    EXPECT_TRUE(m1 == m1);
    EXPECT_TRUE(m2 != m3);
    EXPECT_FALSE(m4 == m3);
}

TEST(MoveTest, MoveTest_BinaryOperator_Tests_Floats) {
    constexpr size_t dim = 4;
    double_pos<dim> p1{M_PI, M_E, M_SQRT2, M_LN10};
    double_pos<dim> p2{4, -2, 7, 2};
    double_move <dim> m1(p1, p2);
    double_move<dim> m3{p1, p2};

    double_pos<dim> p3{1.0, 0.0, 0.0, 1.0};
    double_pos<dim> p4{1.1, 1.1, 1.0, 1.0};
    double_move <dim> m2{p3, p4};

    double_pos<dim> p5{-1., -2, 0.4, 1.7};
    double_pos<dim> p6{0.5, 0.5, 3.5, -1.9};
    double_move <dim> m4{p5, p6};

    //access operator check
    EXPECT_EQ(m1[0], p1);
    EXPECT_EQ(m1[1], p2);

    // manual sum check
    EXPECT_TRUE(m1 == m3);
    EXPECT_TRUE(m1 == m1);
    EXPECT_TRUE(m2 != m3);
    EXPECT_FALSE(m4 == m3);
}
