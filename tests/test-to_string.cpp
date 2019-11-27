struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/to_string.hpp"
#include "picotest/picotest.h"

namespace {

    class ToStringTest : public ::testing::Test {
    };

    TEST_F(ToStringTest, FormatsFloatingPointNumbersNicely) {
        EXPECT_EQ("1.0", tuc::to_string(1.0, 2));
        EXPECT_EQ("1", tuc::to_string(1.0, 1, 0));
        EXPECT_EQ("1.0", tuc::to_string(1.0, 1, 1));
        EXPECT_EQ("10.00", tuc::to_string(10.0, 4));
        EXPECT_EQ("10.0", tuc::to_string(10.0, 3));
        EXPECT_EQ("10", tuc::to_string(10.0, 2, 0));
        EXPECT_EQ("10.0", tuc::to_string(10.0, 2, 1));
        EXPECT_EQ("10", tuc::to_string(10.0, 1, 0));
        EXPECT_EQ("10.0", tuc::to_string(10.0, 1, 1));
        EXPECT_EQ("10.00", tuc::to_string(9.9999, 4));
        EXPECT_EQ("10.0", tuc::to_string(9.9999, 3));
        EXPECT_EQ("10.0", tuc::to_string(9.9999, 2, 1));
        EXPECT_EQ("10", tuc::to_string(9.9999, 2, 0));
        EXPECT_EQ("10.0001", tuc::to_string(10.0001, 6));
        EXPECT_EQ("10.000", tuc::to_string(10.0001, 5));
        EXPECT_EQ("10.00", tuc::to_string(10.0001, 4));
        EXPECT_EQ("10.0", tuc::to_string(10.0001, 3));
        EXPECT_EQ("10.0", tuc::to_string(10.0001, 2, 1));
        EXPECT_EQ("10", tuc::to_string(10.0001, 2, 0));
        EXPECT_EQ("0.000123", tuc::to_string(0.0001234, 3));
        EXPECT_EQ("0.000123", tuc::to_string(0.0001226, 3));
        EXPECT_EQ("-0.000123", tuc::to_string(-0.0001226, 3));
    }

    TEST_F(ToStringTest, FormatsPercentagesNicely) {
        EXPECT_EQ("1", tuc::to_percentage_string(1, 100));
        EXPECT_EQ("1.0", tuc::to_percentage_string(10, 1000));
        EXPECT_EQ("1.00", tuc::to_percentage_string(100, 10000));
        EXPECT_EQ("10.00", tuc::to_percentage_string(1000, 10000));
        EXPECT_EQ("14", tuc::to_percentage_string(1, 7));
        EXPECT_EQ("29", tuc::to_percentage_string(2, 7));
        EXPECT_EQ("0.9", tuc::to_percentage_string(9, 1000));
        EXPECT_EQ("0.009", tuc::to_percentage_string(9, 100000));
        EXPECT_EQ("49.998", tuc::to_percentage_string(33332, 66666));
        EXPECT_EQ("50.000", tuc::to_percentage_string(33333, 66666));
        EXPECT_EQ("50.002", tuc::to_percentage_string(33334, 66666));
        EXPECT_EQ("42.856", tuc::to_percentage_string(33332, 77777));
        EXPECT_EQ("42.857", tuc::to_percentage_string(33333, 77777));
        EXPECT_EQ("42.858", tuc::to_percentage_string(33334, 77777));
    }
}  // namespace
