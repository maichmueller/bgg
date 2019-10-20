//
// Created by michael on 20.10.19.
//

#include "PositionTest.h"
#include "gtest/gtest.h"


TEST(PositionTest, Integers4dim) {
    constexpr size_t dim = 4;
    int_pos<dim> p1{4, 6, 7, 2};
    int_pos<dim> p2{1, -2, 1, 1};
    int_pos<dim> p3{1, -2, 0, 1};
    int_pos<dim> sum{5, 4, 8, 3};
    int_pos<dim> min{3, 8, 6, 1};
    int_pos<dim> mul{4, -12, 7, 2};
    int_pos<dim> div{4, -3, 7, 2};
    int_pos<dim> sum_1_and_2 = p1 + p2;
    int_pos<dim> min_1_and_2 = p1 - p2;
    int_pos<dim> mul_1_and_2 = p1 * p2;
    int_pos<dim> div_1_and_2 = p1 / p2;

    //access operator check
    EXPECT_EQ(p1[0], 4);
    EXPECT_EQ(p1[1], 6);
    EXPECT_EQ(p1[2], 7);
    EXPECT_EQ(p1[3], 2);

    // manual sum check
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    EXPECT_EQ(sum_1_and_2[1], sum[1]);
    EXPECT_EQ(sum_1_and_2[0], sum[2]);
    EXPECT_EQ(sum_1_and_2[1], sum[3]);

    //test Equality operator
    EXPECT_EQ(sum_1_and_2, sum);

    // further operations with eq operator check
    EXPECT_EQ(min_1_and_2[0], min);
    EXPECT_EQ(mul_1_and_2[1], mul);
    EXPECT_EQ(div_1_and_2[1], div);

    // div by zero check
    EXPECT_DEATH(p1 / p3, "Division by Zero.");
}

TEST(PositionTest, Integers7dim) {
    constexpr size_t dim = 7;
    int_pos<dim> p1{1, 6, 7, 0, 23, 9, 0};
    int_pos<dim> p2{4, 2, 0, 1, -3, -8, -1};

    int_pos<dim> sum{5, 8, 7, 1, 20, 1, -1};
    int_pos<dim> min{-3, 4, 7, -1, 26, 17, 1};
    int_pos<dim> mul{4, 12, 0, 0, -69, -72, 0};
    int_pos<dim> sum_1_and_2 = p1 + p2;
    int_pos<dim> min_1_and_2 = p1 - p2;
    int_pos<dim> mul_1_and_2 = p1 * p2;

    // manual sum check
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    EXPECT_EQ(sum_1_and_2[1], sum[0]);
    EXPECT_EQ(sum_1_and_2[2], sum[0]);
    EXPECT_EQ(sum_1_and_2[3], sum[0]);
    EXPECT_EQ(sum_1_and_2[4], sum[0]);
    EXPECT_EQ(sum_1_and_2[5], sum[0]);
    EXPECT_EQ(sum_1_and_2[6], sum[0]);
    //test Equality operator
    EXPECT_EQ(sum_1_and_2, sum);
    // further operations with eq operator check
    EXPECT_EQ(min_1_and_2, min);
    EXPECT_EQ(mul_1_and_2, mul);
    // div by 0
    EXPECT_DEATH(p1 / p2, "Division by Zero.");
}

TEST(PositionTest, Floats4dim) {
    constexpr size_t dim = 4;
    float_pos<dim> p1{M_PI , M_E, M_SQRT2, M_LN10};
    float_pos<dim> p2{1, 2, 0, 1};

    float_pos<dim> sum{5, 8, 7, 3};
    float_pos<dim> min{3, 4, 7, 1};
    float_pos<dim> mul{4, 12, 0, 2};
    float_pos<dim> sum_1_and_2 = p1 + p2;
    float_pos<dim> min_1_and_2 = p1 - p2;
    float_pos<dim> mul_1_and_2 = p1 * p2;
    float_pos<dim> div_1_and_2 = p1 / p2;

    // manual sum check
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    EXPECT_EQ(sum_1_and_2[1], sum[0]);
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    EXPECT_EQ(sum_1_and_2[1], sum[0]);
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    EXPECT_EQ(sum_1_and_2[1], sum[0]);
    EXPECT_EQ(sum_1_and_2[0], sum[0]);
    //test Equality operator
    EXPECT_EQ(sum_1_and_2, sum);
    // further operations with eq operator check
    EXPECT_EQ(min_1_and_2[0], min);
    EXPECT_EQ(mul_1_and_2[1], mul);
    // div by 0
    EXPECT_DEATH(p1 / p2, "Division by Zero.");
}