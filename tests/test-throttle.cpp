struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/throttle.hpp"
#include "picotest/picotest.h"
#include <thread>

namespace {

    class ThrottleTest : public ::testing::Test {
    };

    TEST_F(ThrottleTest, ThrottlesActions) {
        tuc::throttle t;
        int check_counter = 0;
        int action_counter = 0;
        for (int i = 0; i < 100; ++i) {
            ++check_counter;
            if (t.act_if_appropriate(std::chrono::milliseconds(100))) {
                ++action_counter;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        EXPECT_EQ(check_counter, 100);
        EXPECT_GT(action_counter, 5);
        EXPECT_LT(action_counter, 50);
    }

    TEST_F(ThrottleTest, ResetsThrottle) {
        tuc::throttle t;
        int check_counter = 0;
        int action_counter = 0;
        for (int i = 0; i < 100; ++i) {
            ++check_counter;
            t.reset();
            if (t.act_if_appropriate(std::chrono::milliseconds(100))) {
                ++action_counter;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        EXPECT_EQ(check_counter, 100);
        EXPECT_EQ(action_counter, 100);
    }

    TEST_F(ThrottleTest, ProvidesSaneDefaultBehavior) {
        tuc::throttle t;
        t.record_action();
        EXPECT_FALSE(t.should_act());
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        EXPECT_FALSE(t.should_act());
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        EXPECT_TRUE(t.should_act());
    }
}  // namespace
