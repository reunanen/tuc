struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/thread_pool.hpp"
#include "picotest/picotest.h"
#include <numeric> // std::accumulate
#include <array>

namespace {

    class ThreadPoolTest : public ::testing::Test {

    };

    TEST_F(ThreadPoolTest, StartsAndStopsThreadPool) {
        size_t const task_count{ 20 };
        std::atomic<size_t> counter{ 0 };

        tuc::thread_pool tp;

        std::deque<std::future<size_t>> results;

        for (size_t task = 0; task < task_count; ++task) {
            auto future = tp([&counter](size_t task) {
                ++counter;
                return task;
            }, task);
            results.push_back(std::move(future));
        }

        for (size_t task = 0; task < task_count; ++task) {
            auto const result = results[task].get();
            EXPECT_EQ(result, task);
        }

        EXPECT_EQ(counter, task_count);
    }

    TEST_F(ThreadPoolTest, ActuallyHandlesTasksInParallel) {
        auto const task_count{ 20 };
        auto const task_duration_ms{ 1000 };

        tuc::thread_pool tp;

        std::deque<std::future<void>> results;

        auto const t0 = std::chrono::steady_clock::now();

        for (auto task = 0; task < task_count; ++task) {
            results.push_back(tp([task_duration_ms]() {
                return std::this_thread::sleep_for(std::chrono::milliseconds(task_duration_ms));
            }));
        }

        auto const t1 = std::chrono::steady_clock::now();
        auto const d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        EXPECT_LE(d1, 100);

        for (auto task = 0; task < task_count; ++task) {
            results[task].get();
        }

        auto const t2 = std::chrono::steady_clock::now();
        auto const d2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t0).count();
        EXPECT_GE(d2, task_duration_ms);
        auto const hardware_concurrency = std::thread::hardware_concurrency();
        if (hardware_concurrency > 2) {
            EXPECT_LT(d2, task_count * task_duration_ms / 2);
        }
    }

    TEST_F(ThreadPoolTest, CorrectlyPropagatesExceptions) {
        size_t const task_count{ 20 };

        tuc::thread_pool tp;

        std::deque<std::future<void>> results;

        for (size_t task = 0; task < task_count; ++task) {
            results.push_back(tp([task]() {
                throw task;
            }));
        }

        size_t catch_counter = 0;

        for (size_t task = 0; task < task_count; ++task) {
            try {
                results[task].get();
            }
            catch (size_t const exception) {
                EXPECT_EQ(exception, task);
                ++catch_counter;
            }
        }

        EXPECT_EQ(catch_counter, task_count);
    }

    TEST_F(ThreadPoolTest, ProvidesThreadIndex) {
        size_t const thread_pool_size{ 4 };
        size_t const task_count{ 100 };

        std::vector<std::atomic<size_t>> counters(thread_pool_size);
        tuc::thread_pool tp(thread_pool_size);

        std::deque<std::future<void>> results;

        for (size_t task = 0; task < task_count; ++task) {
            results.push_back(tp([&]() {
                ++counters[tp.get_this_thread_index()];
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }));
        }

        EXPECT_EQ(results.size(), task_count);

        auto const get_counter_total = [&counters]() {
            auto const accumulate_counter = [](size_t sum, size_t counter) {
                return sum + counter;
            };
            return std::accumulate(counters.begin(), counters.end(), static_cast<size_t>(0), accumulate_counter);
        };

        // Wait until some results are available
        while (results.size() > task_count * 0.9) {
            results.front().get();
            results.pop_front();
        }

        EXPECT_GE(get_counter_total(), 0.1 * task_count);
        EXPECT_LT(get_counter_total(), 0.5 * task_count);

        // Wait until all results are available
        while (!results.empty()) {
            results.front().get();
            results.pop_front();
        }

        EXPECT_EQ(get_counter_total(), task_count);

        for (auto const& counter : counters)
        {
            // Distribution across threads should be relatively uniform
            size_t const counter_value = counter;
            EXPECT_GE(counter_value, task_count / (thread_pool_size + 2));
            EXPECT_LE(counter_value, task_count / (thread_pool_size - 1));
        }
    }

    TEST_F(ThreadPoolTest, SupportsDeferredExecution) {
        constexpr auto flag_count = 4;
        std::vector<std::atomic<bool>> flags(flag_count);

        tuc::thread_pool tp;

        std::array<std::future<void>, flag_count> results {
            tp(         std::launch::deferred,     [&] { flags[0] = true; }),
            tp(         std::launch::async,        [&] { flags[1] = true; }),
            tuc::launch(std::launch::deferred, tp, [&] { flags[2] = true; }),
            tuc::launch(std::launch::async,    tp, [&] { flags[3] = true; })
        };

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        EXPECT_FALSE(flags[0]);
        EXPECT_TRUE (flags[1]);
        EXPECT_FALSE(flags[2]);
        EXPECT_TRUE (flags[3]);

        results[0].get();
        results[2].get();
        EXPECT_TRUE(flags[0]);
        EXPECT_TRUE(flags[2]);
    }

}  // namespace
