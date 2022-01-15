struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/thread.hpp"
#include "picotest/picotest.h"
#include <deque>
#include <array>
#include <atomic>
#include <numeric>

namespace {

    class ThreadTest : public ::testing::Test {

    };

    TEST_F(ThreadTest, JoinsThreadAutomatically) {
        auto const nop = []() {};
        tuc::thread t1(nop);
        tuc::thread t2(std::move(std::thread(nop)));
    }

    TEST_F(ThreadTest, SetsThreadPriority) {
        unsigned int const hardware_concurrency = std::thread::hardware_concurrency();

        std::array<double, 2> low_to_normal_ratios = {
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        };

        for (int actually_set_low_priority = 0; actually_set_low_priority <= 1; ++actually_set_low_priority) {
            std::vector<std::atomic<uintmax_t>> low_priority_counters(hardware_concurrency);
            std::vector<std::atomic<uintmax_t>> normal_priority_counters(hardware_concurrency);

            {
                std::deque<tuc::thread> low_priority_threads;
                std::deque<tuc::thread> normal_priority_threads;

                std::atomic<bool> done = false;

                auto const busy_loop = [&done, hardware_concurrency](std::atomic<uintmax_t>& counter) {
                    auto const max_value = (std::numeric_limits<uintmax_t>::max)() / hardware_concurrency;
                    while (!done) {
                        ++counter;
                        if (counter >= max_value) {
                            break; // normally not reached
                        }
                    }
                };

                auto const common_start_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                for (unsigned int i = 0; i < hardware_concurrency; ++i) {
                    low_priority_threads.emplace_back([&](unsigned int i) {
                        if (actually_set_low_priority) {
                            tuc::set_current_thread_to_idle_priority();
                        }
                        std::this_thread::sleep_until(common_start_time);
                        busy_loop(low_priority_counters[i]);
                    }, i);
                    normal_priority_threads.emplace_back([&](unsigned int i) {
                        std::this_thread::sleep_until(common_start_time);
                        busy_loop(normal_priority_counters[i]);
                    }, i);
                }

#ifdef WIN32
                std::this_thread::sleep_for(std::chrono::seconds(10));
#else // WIN32
                std::this_thread::sleep_for(std::chrono::seconds(60));
#endif // WIN32

                auto const get_total = [](auto const& counters) {
                    return std::accumulate(counters.begin(), counters.end(), static_cast<uintmax_t>(0));
                };
                auto const normal_priority_total = get_total(normal_priority_counters);
                auto const low_priority_total = get_total(low_priority_counters);

                auto const low_to_normal_ratio = low_priority_total / static_cast<double>(normal_priority_total);

                low_to_normal_ratios[actually_set_low_priority] = low_to_normal_ratio;

                done = true; // stop the threads

                EXPECT_GT(normal_priority_total, static_cast<uintmax_t>(1'000'000));

#ifdef WIN32
                if (hardware_concurrency == 2) {
                    ; // Difficult to get Appveyor builds to behave predictably :(
                }
                else {
                    if (actually_set_low_priority) {
                        EXPECT_LT(low_to_normal_ratio, 0.05);
                    }
                    else {
                        EXPECT_LT(low_to_normal_ratio, 1.5);
                        EXPECT_GT(low_to_normal_ratio, 0.5);
                    }
                }
#else // WIN32
                if (actually_set_low_priority) {
                    EXPECT_LT(low_to_normal_ratio, 0.7);
                }
                else {
                    EXPECT_LT(low_to_normal_ratio, 1.25);
                    EXPECT_GT(low_to_normal_ratio, 0.75);
                }
#endif // WIN32
            }
        }

#ifdef WIN32
        if (hardware_concurrency == 2) {
            ; // Difficult to get Appveyor builds to behave predictably
        }
        else {
            EXPECT_GT(low_to_normal_ratios[0], low_to_normal_ratios[1]);
        }
#else // WIN32
        EXPECT_GT(low_to_normal_ratios[0], low_to_normal_ratios[1]);
#endif // WIN32
    }

}  // namespace
