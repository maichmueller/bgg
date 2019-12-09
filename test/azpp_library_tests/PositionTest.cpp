#include "PositionTest.h"
#include "gtest/gtest.h"
#include <cmath>


TEST(PositionTest, PositionTest_CompOperators_Ints_4D) {
    constexpr size_t dim = 4;
    int_pos<dim> p1{4, 6, 7, 2};
    int_pos<dim> p2{4, -2, 7, 2};
    int_pos<dim> p3{4, -2, 5, 1};
    int_pos<dim> p4{4, -2, 5, 0};

    //access operator check
    EXPECT_EQ(p1[0], 4);
    EXPECT_EQ(p1[1], 6);
    EXPECT_EQ(p1[2], 7);
    EXPECT_EQ(p1[3], 2);

    EXPECT_FALSE(p1 != p1);
    EXPECT_FALSE(p2 != p2);
    EXPECT_FALSE(p3 != p3);
    EXPECT_FALSE(p4 != p4);

    EXPECT_FALSE(p1 <= p2);
    EXPECT_FALSE(p1 <= p3);
    EXPECT_FALSE(p1 <= p4);
    EXPECT_FALSE(p1 < p2);
    EXPECT_FALSE(p1 < p3);
    EXPECT_FALSE(p1 < p4);

    EXPECT_FALSE(p2 <= p3);
    EXPECT_FALSE(p2 <= p4);
    EXPECT_FALSE(p2 < p3);
    EXPECT_FALSE(p2 < p4);

    EXPECT_FALSE(p3 <= p4);
    EXPECT_FALSE(p3 < p4);

    EXPECT_TRUE(p1 == p1);
    EXPECT_TRUE(p2 == p2);
    EXPECT_TRUE(p3 == p3);
    EXPECT_TRUE(p4 == p4);

    EXPECT_TRUE(p1 > p2);
    EXPECT_TRUE(p1 > p3);
    EXPECT_TRUE(p1 > p4);
    EXPECT_TRUE(p1 >= p2);
    EXPECT_TRUE(p1 >= p3);
    EXPECT_TRUE(p1 >= p4);

    EXPECT_TRUE(p2 > p3);
    EXPECT_TRUE(p2 > p4);
    EXPECT_TRUE(p2 >= p3);
    EXPECT_TRUE(p2 >= p4);

    EXPECT_TRUE(p3 > p4);
    EXPECT_TRUE(p3 >= p4);
}

TEST(PositionTest, PositionTest_CompOperators_Floats_4D) {
    constexpr size_t dim = 4;
    float_pos<dim> p1{4.4, 6.2, 7.8, 2.2};
    float_pos<dim> p2{4.4, 5.78, 7.673, 2.1};
    float_pos<dim> p3{4.4, 5.78, -1.45, 5.000001};
    float_pos<dim> p4{4.4, 5.78, -1.45, 5.000000};

    //access operator check
    EXPECT_EQ(p1[0], static_cast<float>(4.4));
    EXPECT_EQ(p1[1], static_cast<float>(6.2));
    EXPECT_EQ(p1[2], static_cast<float>(7.8));
    EXPECT_EQ(p1[3], static_cast<float>(2.2));

    EXPECT_FALSE(p1 != p1);
    EXPECT_FALSE(p2 != p2);
    EXPECT_FALSE(p3 != p3);
    EXPECT_FALSE(p4 != p4);

    EXPECT_FALSE(p1 <= p2);
    EXPECT_FALSE(p1 <= p3);
    EXPECT_FALSE(p1 <= p4);
    EXPECT_FALSE(p1 < p2);
    EXPECT_FALSE(p1 < p3);
    EXPECT_FALSE(p1 < p4);

    EXPECT_FALSE(p2 <= p3);
    EXPECT_FALSE(p2 <= p4);
    EXPECT_FALSE(p2 < p3);
    EXPECT_FALSE(p2 < p4);

    EXPECT_FALSE(p3 <= p4);
    EXPECT_FALSE(p3 < p4);

    EXPECT_TRUE(p1 == p1);
    EXPECT_TRUE(p2 == p2);
    EXPECT_TRUE(p3 == p3);
    EXPECT_TRUE(p4 == p4);

    EXPECT_TRUE(p1 > p2);
    EXPECT_TRUE(p1 > p3);
    EXPECT_TRUE(p1 > p4);
    EXPECT_TRUE(p1 >= p2);
    EXPECT_TRUE(p1 >= p3);
    EXPECT_TRUE(p1 >= p4);

    EXPECT_TRUE(p2 > p3);
    EXPECT_TRUE(p2 > p4);
    EXPECT_TRUE(p2 >= p3);
    EXPECT_TRUE(p2 >= p4);

    EXPECT_TRUE(p3 > p4);
    EXPECT_TRUE(p3 >= p4);
}

TEST(PositionTest, PositionTest_BinaryOperators_Ints_4D) {
    constexpr size_t dim = 4;
    int_pos<dim> p1{4, 6, 7, 2};
    int_pos<dim> p2{1, -2, 1, 1};
    int_pos<dim> p3{1, -2, 0, 1};

    // define expected values for operations
    int_pos<dim> sum{5, 4, 8, 3};
    int_pos<dim> min{3, 8, 6, 1};
    int_pos<dim> mul{4, -12, 7, 2};
    int_pos<dim> div{4, -3, 7, 2};

    int_pos<dim> p1_mul_3{12, 18, 21, 6};
    int_pos<dim> p1_mul_2_2{static_cast<int>(4 * 2.2), static_cast<int>(6 * 2.2), static_cast<int>(7 * 2.2), static_cast<int>(2 * 2.2)};
    int_pos<dim> p1_div_2_2{static_cast<int>(4 / 2.2), static_cast<int>(6 / 2.2), static_cast<int>(7 / 2.2), static_cast<int>(2 / 2.2)};

    // further binary operations check
    EXPECT_EQ(p1 + p2, sum);
    EXPECT_EQ(p1 - p2, min);
    EXPECT_EQ(p1 * p2, mul);
    EXPECT_EQ(p1 / p2, div);

    EXPECT_EQ(p1 * 3, p1_mul_3);
    EXPECT_EQ(3 * p1, p1_mul_3);
    EXPECT_EQ(p1 * 2.2, p1_mul_2_2);
    EXPECT_EQ(2.2 * p1 , p1_mul_2_2);
    EXPECT_EQ(p1 / 2.2, p1_div_2_2);
    EXPECT_EQ(2.2 / p1, int_pos<dim>(static_cast<int>(2.2 / 4), static_cast<int>(2.2 / 6), static_cast<int>(2.2 / 7), static_cast<int>(2.2 / 2)));

    // div by zero check
    EXPECT_DEATH(p1 / p3, "");

    EXPECT_EQ(p1.invert(std::array{0, 0, 0, 0}, std::array{100, 20, 100, 50}), int_pos<dim>(96, 14, 93, 48));
    EXPECT_EQ(p1.invert(std::array{-10, 4, 3, 0}, std::array{100, 20, 100, 50}), int_pos<dim>(86, 18, 96, 48));
}

TEST(PositionTest, PositionTest_BinaryOperators_Ints_7D) {
    constexpr size_t dim = 7;
    int_pos<dim> p1{1, 6, 7, 0, 23, 9, 0};
    int_pos<dim> p2{4, 2, 0, 1, -3, -8, -1};

    int_pos<dim> sum{5, 8, 7, 1, 20, 1, -1};
    int_pos<dim> min{-3, 4, 7, -1, 26, 17, 1};
    int_pos<dim> mul{4, 12, 0, 0, -69, -72, 0};

    EXPECT_EQ(p1 + p2, sum);
    EXPECT_EQ(p1 - p2, min);
    EXPECT_EQ(p1 * p2, mul);
    EXPECT_DEATH(p1 / p2, "");

}

TEST(PositionTest, PositionTest_BinaryOperators_Doubles_4D) {
    constexpr size_t dim = 4;
    double_pos <dim> p1{M_PI, M_E, M_SQRT2, M_LN10};
    double_pos <dim> p2{1.34, 2.2, M_LN10, -1.635};

    double_pos <dim> sum{M_PI + 1.34,
                       M_E + 2.2,
                       M_SQRT2 + M_LN10,
                       M_LN10 + (-1.635)};
    double_pos <dim> min{M_PI - 1.34,
                       M_E - 2.2,
                       M_SQRT2 - M_LN10,
                       M_LN10 - (-1.635)};
    double_pos <dim> mul{M_PI * 1.34,
                       M_E * 2.2,
                       M_SQRT2 * M_LN10,
                       M_LN10 * (-1.635)};
    double_pos <dim> div{M_PI / 1.34,
                       M_E / 2.2,
                       M_SQRT2 / M_LN10,
                       M_LN10 / (-1.635)};

    EXPECT_EQ(p1 + p2, sum);
    EXPECT_EQ(p1 - p2, min);
    EXPECT_EQ(p1 * p2, mul);
    EXPECT_EQ(p1 / p2, div);
}