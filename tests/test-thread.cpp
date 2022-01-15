struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/thread.hpp"
#include "picotest/picotest.h"
#include <deque>
#include <atomic>

namespace {

    class ThreadTest : public ::testing::Test {

    };

    TEST_F(ThreadTest, SetsThreadPriority) {
        for (int actually_set_low_priority = 0; actually_set_low_priority <= 1; ++actually_set_low_priority) {
            std::deque<std::thread> low_priority_threads;
            std::deque<std::thread> normal_priority_threads;

            std::atomic<uintmax_t> low_priority_counter = 0;
            std::atomic<uintmax_t> normal_priority_counter = 0;

            const int max_value_per_thread = 10000000;

            const auto increment_counter_multiple_times = [max_value_per_thread](std::atomic<uintmax_t>& counter) {
                for (int i = 0; i < max_value_per_thread; ++i) {
                    ++counter;
                }
            };

            const auto hardware_concurrency = std::thread::hardware_concurrency();

            for (unsigned int i = 0; i < hardware_concurrency; ++i) {
                low_priority_threads.push_back(std::thread([&]() {
                    if (actually_set_low_priority) {
                        tuc::set_current_thread_to_idle_priority();
                    }
                    increment_counter_multiple_times(low_priority_counter);
                }));
                normal_priority_threads.push_back(std::thread([&]() {
                    increment_counter_multiple_times(normal_priority_counter);
                }));
            }

            const auto max_total_value = hardware_concurrency * max_value_per_thread;

            while (normal_priority_counter < max_total_value) {
                ;
            }

            const uintmax_t low_priority_value = low_priority_counter;
            if (actually_set_low_priority) {
                EXPECT_LT(low_priority_value, max_total_value * 0.2);
            }
            else {
                EXPECT_GT(low_priority_value, max_total_value * 0.8);
            }

            for (auto& t : normal_priority_threads) {
                t.join();
            }
            for (auto& t : low_priority_threads) {
                t.join();
            }
        }
    }

}  // namespace
