struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/functional.hpp"
#include "picotest/picotest.h"
#include <deque>

namespace {

    class FunctionalTest : public ::testing::Test {
    protected:
        std::vector<int> const input{ 1, 1, 2, 3, 5, 8, 13 };
        std::function<int(int)> const map_function = [](int input) { return 2 * input; };
        std::function<int(int)> const filter_function = [](int input) { return input % 2 > 0; };

        FunctionalTest()
        {
            std::transform(input.begin(), input.end(), std::back_inserter(desired_map_output), map_function);
            std::copy_if(input.begin(), input.end(), std::back_inserter(desired_filter_output), filter_function);
            std::set_difference(input.begin(), input.end(), desired_filter_output.begin(), desired_filter_output.end(), std::back_inserter(desired_remove_if_output));
        }

        std::deque<int> desired_map_output;
        std::deque<int> desired_filter_output;
        std::deque<int> desired_remove_if_output;
    };

    TEST_F(FunctionalTest, MapsVectorToVector) {
        std::vector<int> const desired_map_output_as_vector(desired_map_output.begin(), desired_map_output.end());

        auto const output = tuc::map<std::vector<int>>(input, map_function);

        EXPECT_EQ(output, desired_map_output_as_vector);
    }

    TEST_F(FunctionalTest, MapsVectorToDeque) {
        auto const output = tuc::map<std::deque<int>>(input, map_function);

        EXPECT_EQ(output, desired_map_output);
    }

    TEST_F(FunctionalTest, MapsDequeToVector) {
        std::deque<int> const input_as_deque(input.begin(), input.end());
        std::vector<int> const desired_map_output_as_vector(desired_map_output.begin(), desired_map_output.end());

        auto const output = tuc::map<std::vector<int>>(input_as_deque, map_function);

        EXPECT_EQ(output, desired_map_output_as_vector);
    }

    TEST_F(FunctionalTest, MapsDequeToDeque) {
        std::deque<int> const input_as_deque(input.begin(), input.end());

        auto const output = tuc::map<std::deque<int>>(input_as_deque, map_function);

        EXPECT_EQ(output, desired_map_output);
    }

    TEST_F(FunctionalTest, FiltersVector) {
        std::vector<int> const desired_filter_output_as_vector(desired_filter_output.begin(), desired_filter_output.end());

        auto const output = tuc::filter<std::vector<int>>(input, filter_function);

        EXPECT_EQ(output, desired_filter_output_as_vector);
    }

    TEST_F(FunctionalTest, FiltersDeque) {
        std::deque<int> const input_as_deque(input.begin(), input.end());

        auto const output = tuc::filter<std::deque<int>>(input_as_deque, filter_function);

        EXPECT_EQ(output, desired_filter_output);
    }

    TEST_F(FunctionalTest, RemovesFromVector) {
        std::vector<int> input_and_output(input.begin(), input.end());
        std::vector<int> const desired_remove_if_output_as_vector(desired_remove_if_output.begin(), desired_remove_if_output.end());

        tuc::remove_if(input_and_output, filter_function);

        EXPECT_EQ(input_and_output, desired_remove_if_output_as_vector);
    }

    TEST_F(FunctionalTest, RemovesFromDeque) {
        std::deque<int> input_and_output(input.begin(), input.end());

        tuc::remove_if(input_and_output, filter_function);

        EXPECT_EQ(input_and_output, desired_remove_if_output);
    }

}  // namespace
