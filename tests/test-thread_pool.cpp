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
        auto constexpr flag_count = 4;
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

    TEST_F(ThreadPoolTest, LaunchesTasksInChunks) {
        size_t const task_count{ std::thread::hardware_concurrency() * 4 + 3 };

        tuc::thread_pool tp;

        std::vector<size_t> processing_thread_indexes(task_count, std::numeric_limits<size_t>::max());

        auto const task = [&processing_thread_indexes, &tp](size_t task) {
            processing_thread_indexes[task] = tp.get_thread_index(std::this_thread::get_id());
            return task;
        };

        std::vector<std::future<size_t>> results = tp.launch_in_chunks(task, task_count);

        ASSERT_EQ(results.size(), task_count);

        for (size_t i = 0; i < task_count; ++i) {
            EXPECT_EQ(results[i].get(), i);
        }

        auto const default_desired_chunk_size = tp.get_default_desired_chunk_size(task_count);

        EXPECT_LT(default_desired_chunk_size, tuc::divide_rounding_up(task_count, tp.get_thread_count()));

        auto const min_default_desired_chunk_size = tp.get_default_desired_chunk_size(task_count, -std::numeric_limits<double>::infinity());
        auto const max_default_desired_chunk_size = tp.get_default_desired_chunk_size(task_count,  std::numeric_limits<double>::infinity());

        EXPECT_EQ(min_default_desired_chunk_size, static_cast<size_t>(1));
        EXPECT_EQ(max_default_desired_chunk_size, tuc::divide_rounding_up(task_count, tp.get_thread_count()));

        for (size_t i = 0; i < task_count; ++i) {
            auto const reference = (i / default_desired_chunk_size) * default_desired_chunk_size;
            EXPECT_EQ(processing_thread_indexes[i], processing_thread_indexes[reference]);
            EXPECT_LT(processing_thread_indexes[i], std::numeric_limits<size_t>::max());
        }
    }

    TEST_F(ThreadPoolTest, LaunchesTasksInChunksReturningSingleFutureForEachChunk) {
        bool constexpr is_64_bit_build = sizeof(void*) >= 8; // see https://stackoverflow.com/a/6706162

        size_t constexpr task_count_multiplier = is_64_bit_build ? 1000000 : 10000;

        size_t const task_count{ std::thread::hardware_concurrency() * task_count_multiplier + 3 };

        tuc::thread_pool tp;

        std::vector<size_t> processing_thread_indexes(task_count, std::numeric_limits<size_t>::max());

        auto const task = [&processing_thread_indexes, &tp](size_t task) {
            processing_thread_indexes[task] = tp.get_thread_index(std::this_thread::get_id());
            return task;
        };

        auto futures = tp.launch_in_chunks_returning_single_future_for_each_chunk(task, task_count);

        auto const thread_count = tp.get_thread_count();

        EXPECT_GT(futures.size(), thread_count);
        EXPECT_LT(futures.size(), task_count);

        size_t expected_task = 0;

        for (auto& future : futures) {
            auto const results = future.get();
            for (auto const& result : results) {
                EXPECT_EQ(result, expected_task);
                ++expected_task;
            }
        }

        EXPECT_EQ(expected_task, task_count);

        std::vector<size_t> task_count_by_thread_index(thread_count);

        for (size_t i = 0; i < task_count; ++i) {
            auto const thread_index = processing_thread_indexes[i];
            ASSERT_LT(thread_index, thread_count);

            ++task_count_by_thread_index[thread_index];
        }

        if (std::thread::hardware_concurrency() > 4) {
            // each thread should have done something at least... but not all the work
            for (size_t i = 0; i < thread_count; ++i) {
                auto const thread_task_count = task_count_by_thread_index[i];
                EXPECT_GT(thread_task_count, static_cast<size_t>(0));
                EXPECT_LT(thread_task_count, task_count);

                auto const min_thread_task_count = tuc::divide_rounding_to_closest(task_count, thread_count * 8);
                auto const max_thread_task_count = tuc::divide_rounding_to_closest(task_count * 8, thread_count);
                EXPECT_GE(thread_task_count, min_thread_task_count);
                EXPECT_LE(thread_task_count, max_thread_task_count);
            }
        }
    }

    TEST_F(ThreadPoolTest, LaunchesTasksInChunksReturningSingleFutureForEachChunkEvenWhenFunctionReturnTypeIsVoid) {
        size_t const task_count{ std::thread::hardware_concurrency() * 1000 + 3 };

        tuc::thread_pool tp;

        auto const task = [](size_t) {};

        std::vector<size_t> arguments(task_count);
        std::iota(arguments.begin(), arguments.end(), 0);

        auto futures = tp.launch_in_chunks_returning_single_future_for_each_chunk(task, arguments);

        for (auto& future : futures) {
            future.get();
            static_assert(std::is_void<decltype(future.get())>::value);
        }
    }

    TEST_F(ThreadPoolTest, ChangesThreadPoolSize) {
        tuc::thread_pool tp;
        EXPECT_EQ(tp.get_thread_count(), std::thread::hardware_concurrency());

        tp.set_thread_count(1u);
        EXPECT_EQ(tp.get_thread_count(), 1u);

        tp.set_thread_count(4u);
        EXPECT_EQ(tp.get_thread_count(), 4u);

        tp.set_thread_count(2u);
        EXPECT_EQ(tp.get_thread_count(), 2u);
    }

}  // namespace
