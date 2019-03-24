struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/functional.hpp"
#include "picotest/picotest.h"
#include <deque>

namespace {

    class FunctionalTest : public ::testing::Test {
    protected:
        std::vector<int> const input{ 1, 1, 2, 3, 5, 8, 13 };
        std::function<int(int)> const function = [](int input) { return 2 * input; };

        FunctionalTest()
        {
            std::transform(input.begin(), input.end(), std::back_inserter(desired_output), function);
        }

        std::deque<int> desired_output;
    };

    TEST_F(FunctionalTest, MapsVectorToVector) {
        std::vector<int> const desired_output_as_vector(desired_output.begin(), desired_output.end());

        auto const output = tuc::map<std::vector<int>>(input, function);

        EXPECT_EQ(output, desired_output_as_vector);
    }

    TEST_F(FunctionalTest, MapsVectorToDeque) {
        auto const output = tuc::map<std::deque<int>>(input, function);

        EXPECT_EQ(output, desired_output);
    }

    TEST_F(FunctionalTest, MapsDequeToVector) {
        std::deque<int> const input_as_deque(input.begin(), input.end());
        std::vector<int> const desired_output_as_vector(desired_output.begin(), desired_output.end());

        auto const output = tuc::map<std::vector<int>>(input_as_deque, function);

        EXPECT_EQ(output, desired_output_as_vector);
    }

    TEST_F(FunctionalTest, MapsDequeToDeque) {
        std::deque<int> const input_as_deque(input.begin(), input.end());

        auto const output = tuc::map<std::deque<int>>(input_as_deque, function);

        EXPECT_EQ(output, desired_output);
    }

}  // namespace
