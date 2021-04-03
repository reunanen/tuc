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

    TEST_F(NumericTest, RoundsEasily) {
        EXPECT_EQ(3, tuc::round(3.1));
        EXPECT_EQ(3u, tuc::round<unsigned int>(3.1));
        EXPECT_EQ(255, tuc::round<uint8_t>(255.1));
        EXPECT_EQ(0, tuc::round<uint8_t>(-0.1));
        try {
            tuc::round<uint8_t>(255.5);
            EXPECT_TRUE(false);
        }
        catch (std::exception&) {
            ; // this is where we want to be
        }
    }

    template <typename Output, typename Input>
    void expect_overflow(Input input) {
        try {
            const auto rounded = tuc::round<Output>(input);
            EXPECT_TRUE(false);
        }
        catch (std::exception& e) {
            const std::string what = e.what();
            EXPECT_NE(what.find("overflow"), std::string::npos);
        }
    }

    TEST_F(NumericTest, DetectsOverflow) {
        expect_overflow<int>(1e20);
        expect_overflow<unsigned int>(-5.0);
    }

    TEST_F(NumericTest, InterpolatesLinearly) {
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 0.0), 2.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 1.0), 5.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 0.5), 3.5);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, -1.0), -1.0);
        EXPECT_EQ(tuc::lerp(2.0, 5.0, 2.0), 8.0);
    }

    TEST(NumericTest, DeterminesSign) {
        EXPECT_EQ(tuc::sign(2), 1);
        EXPECT_EQ(tuc::sign(1), 1);
        EXPECT_EQ(tuc::sign(0), 0);
        EXPECT_EQ(tuc::sign(-2), -1);
        EXPECT_EQ(tuc::sign(2.0), 1);
        EXPECT_EQ(tuc::sign(0.1), 1);
        EXPECT_EQ(tuc::sign(0.0), 0);
        EXPECT_EQ(tuc::sign(-2.0), -1);
    }

    TEST(NumericTest, CalculatesMeans) {
        std::vector<double> const numbers = { 1.0, 3.0, 9.0 };
        EXPECT_NEAR(tuc::arithmetic_mean(numbers.begin(), numbers.end()), 4.333333, 1e-6);
        EXPECT_EQ(tuc::geometric_mean(numbers.begin(), numbers.end()), 3.0);
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), std::numeric_limits<double>::lowest()), *std::min_element(numbers.begin(), numbers.end()), 1e-20);
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), -1), 2.0769231, 1e-6); // harmonic mean
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), 0), tuc::geometric_mean(numbers.begin(), numbers.end()), 1e-20);
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), 1), tuc::arithmetic_mean(numbers.begin(), numbers.end()), 1e-20);
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), 2), 5.50757, 1e-5);
        EXPECT_NEAR(tuc::power_mean(numbers.begin(), numbers.end(), 300), 8.9671, 1e-4);
        EXPECT_EQ(tuc::power_mean(numbers.begin(), numbers.end(), std::numeric_limits<double>::max()), *std::max_element(numbers.begin(), numbers.end()));

        for (int p10 = -3000; p10 < 3000; ++p10) {
            double const p = p10 / 10.0;
            EXPECT_LT(tuc::power_mean(numbers.begin(), numbers.end(), p), tuc::power_mean(numbers.begin(), numbers.end(), p + 1));
        }
    }

    TEST(NumericTest, ProvidesUnambiguousClampFunctionality) {
        EXPECT_EQ(tuc::unambiguous_clamp<int>().low(5).high(10).value(3), 5);
        EXPECT_EQ(tuc::unambiguous_clamp<int>().low(5).high(10).value(12), 10);
        EXPECT_EQ(tuc::unambiguous_clamp<int>().low(5).high(10).value(7), 7);
    }

}  // namespace
