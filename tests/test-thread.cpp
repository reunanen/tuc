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

    // Keeps the counters of different threads on different cache lines, so that the threads do not slow each other down
    struct padded_counter {
        std::atomic<uintmax_t> value{ 0 };
        char padding[64 - sizeof(std::atomic<uintmax_t>)];
    };

    TEST_F(ThreadTest, JoinsThreadAutomatically) {
        auto const nop = []() {};
        tuc::thread t1(nop);
        tuc::thread t2(std::move(std::thread(nop)));
    }

    TEST_F(ThreadTest, SetsThreadPriority) {
        unsigned int const hardware_concurrency = std::thread::hardware_concurrency();

        std::array<double, 2> idle_priority_to_normal_priority_ratios = {
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        };

        for (int actually_set_idle_priority = 0; actually_set_idle_priority <= 1; ++actually_set_idle_priority) {
            std::vector<padded_counter> idle_priority_counters(hardware_concurrency);
            std::vector<padded_counter> normal_priority_counters(hardware_concurrency);

            {
                std::deque<tuc::thread> idle_priority_threads;
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
                    idle_priority_threads.emplace_back([&](unsigned int i) {
                        if (actually_set_idle_priority) {
                            tuc::set_current_thread_to_idle_priority();
                        }
                        std::this_thread::sleep_until(common_start_time);
                        busy_loop(idle_priority_counters[i].value);
                    }, i);
                    normal_priority_threads.emplace_back([&](unsigned int i) {
                        std::this_thread::sleep_until(common_start_time);
                        busy_loop(normal_priority_counters[i].value);
                    }, i);
                }

#ifdef WIN32
                std::this_thread::sleep_for(std::chrono::seconds(10));
#else // WIN32
                std::this_thread::sleep_for(std::chrono::seconds(60));
#endif // WIN32

                auto const get_total = [](auto const& counters) {
                    return std::accumulate(counters.begin(), counters.end(), static_cast<uintmax_t>(0), [](uintmax_t total, padded_counter const& counter) {
                        return total + counter.value;
                    });
                };
                auto const normal_priority_total = get_total(normal_priority_counters);
                auto const idle_priority_total = get_total(idle_priority_counters);

                auto const idle_to_normal_ratio = idle_priority_total / static_cast<double>(normal_priority_total);

                idle_priority_to_normal_priority_ratios[actually_set_idle_priority] = idle_to_normal_ratio;

                done = true; // stop the threads

                EXPECT_GT(normal_priority_total, static_cast<uintmax_t>(1'000'000));

#ifdef WIN32
                if (hardware_concurrency == 2) {
                    ; // Difficult to get Appveyor builds to behave predictably :(
                }
                else {
                    if (actually_set_idle_priority) {
                        EXPECT_LT(idle_to_normal_ratio, 0.05);
                    }
                    else {
                        EXPECT_LT(idle_to_normal_ratio, 1.5);
                        EXPECT_GT(idle_to_normal_ratio, 0.5);
                    }
                }
#else // WIN32
                if (actually_set_idle_priority) {
                    EXPECT_LT(idle_to_normal_ratio, 0.75);
                }
                else {
                    EXPECT_LT(idle_to_normal_ratio, 1.25);
                    EXPECT_GT(idle_to_normal_ratio, 0.75);
                }
#endif // WIN32
            }
        }

#ifdef WIN32
        if (hardware_concurrency == 2) {
            ; // Difficult to get Appveyor builds to behave predictably
        }
        else {
            EXPECT_GT(idle_priority_to_normal_priority_ratios[0], idle_priority_to_normal_priority_ratios[1]);
        }
#else // WIN32
        EXPECT_GT(idle_priority_to_normal_priority_ratios[0], idle_priority_to_normal_priority_ratios[1]);
#endif // WIN32
    }

}  // namespace
