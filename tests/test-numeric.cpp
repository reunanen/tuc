struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/numeric.hpp"
#include "picotest/picotest.h"

namespace {

    class NumericTest : public ::testing::Test {
    };

    TEST_F(NumericTest, DividesRoundingUp) {
        EXPECT_EQ(3, tuc::divide_rounding_up(5, 2));
        EXPECT_EQ(-2, tuc::divide_rounding_up(-5, 2));
        EXPECT_EQ(0, tuc::divide_rounding_up(0, 2));
    }

    TEST_F(NumericTest, DividesRoundingToClosest) {
        EXPECT_EQ(3, tuc::divide_rounding_to_closest(5, 2));
        EXPECT_EQ(-3, tuc::divide_rounding_to_closest(-5, 2));
        EXPECT_EQ(0, tuc::divide_rounding_to_closest(0, 2));
        EXPECT_EQ(2, tuc::divide_rounding_to_closest(7, 3));
        EXPECT_EQ(3, tuc::divide_rounding_to_closest(8, 3));
        EXPECT_EQ(-2, tuc::divide_rounding_to_closest(-7, 3));
        EXPECT_EQ(-3, tuc::divide_rounding_to_closest(8, -3));
    }

    TEST_F(NumericTest, DividesRoundingDown) {
        EXPECT_EQ(2, tuc::divide_rounding_down(5, 2));
        EXPECT_EQ(-3, tuc::divide_rounding_down(-5, 2));
        EXPECT_EQ(0, tuc::divide_rounding_down(0, 2));
    }

    TEST_F(NumericTest, InterpolatesLinearly) {
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 0.0), 2.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 1.0), 5.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 0.5), 3.5);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, -1.0), -1.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 2.0), 8.0);
    }

}  // namespace
