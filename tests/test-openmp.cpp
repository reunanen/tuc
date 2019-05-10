struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/openmp.hpp"
#include "picotest/picotest.h"
#include <thread>
#include <mutex>
#include <deque>
#include <numeric> // std::iota

namespace {

    class OpenMpTest : public ::testing::Test {
    };

    constexpr int loops = 100;

    std::mutex mutex;

    std::deque<int> result;

    const void push_i(int i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(loops - i));
        
        std::lock_guard<std::mutex> lock(mutex);
        result.push_back(i);
    }

    const std::deque<int> expected_sequential_result() {
        std::deque<int> sequential_result(loops);
        std::iota(sequential_result.begin(), sequential_result.end(), 0);
        return sequential_result;
    }

    TEST_F(OpenMpTest, RunsInParallel) {
        result.clear();
        tuc::openmp::maybe_parallelize_for_loop(push_i, loops, true);
        
        EXPECT_EQ(result.size(), loops); // The size should match
        EXPECT_NE(result, expected_sequential_result());     // The contents should not match, because of the sleeps
    }

    TEST_F(OpenMpTest, RunsSequentially) {
        result.clear();
        tuc::openmp::maybe_parallelize_for_loop(push_i, loops, false);

        EXPECT_EQ(result, expected_sequential_result()); // The contents should match
    }
}  // namespace
