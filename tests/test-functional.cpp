struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/functional.hpp"
#include "picotest/picotest.h"
#include <iterator>
#include <deque>
#include <random>

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

    TEST_F(FunctionalTest, SortsAscending) {
        typedef std::tuple<int, int, int> SortTestItem;

        std::vector<SortTestItem> data = {
            { 0, 3, 8 }, { 1, 2, -1 }, { 3, 1, 5 }, { 2, 4, -2 }
        };

        std::vector<SortTestItem> const data_sorted_by_index_0 = {
            { 0, 3, 8 }, { 1, 2, -1 }, { 2, 4, -2 }, { 3, 1, 5 }
        };

        std::vector<SortTestItem> const data_sorted_by_index_1 = {
            { 3, 1, 5 }, { 1, 2, -1 }, { 0, 3, 8 }, { 2, 4, -2 }
        };

        std::vector<SortTestItem> const data_sorted_by_index_2 = {
            { 2, 4, -2 }, { 1, 2, -1 }, { 3, 1, 5 }, { 0, 3, 8 },
        };

        tuc::sort_ascending(data, [](SortTestItem const& item) { return std::get<0>(item); });

        EXPECT_EQ(data, data_sorted_by_index_0);

        tuc::sort_ascending(data, [](SortTestItem const& item) { return std::get<1>(item); });

        EXPECT_EQ(data, data_sorted_by_index_1);

        tuc::sort_ascending(data, [](SortTestItem const& item) { return std::get<2>(item); });

        EXPECT_EQ(data, data_sorted_by_index_2);

        auto const const_data = data;

        auto const copy_sorted_by_index_1 = tuc::sort_ascending(const_data, [](SortTestItem const& item) { return std::get<1>(item); });

        EXPECT_EQ(copy_sorted_by_index_1, data_sorted_by_index_1);
    }

    TEST_F(FunctionalTest, SortsDescending) {
        typedef std::tuple<int, int, int> SortTestItem;

        std::vector<SortTestItem> data = {
            { 0, 3, 8 }, { 1, 2, -1 }, { 3, 1, 5 }, { 2, 4, -2 }
        };

        std::vector<SortTestItem> const data_sorted_by_index_1 = {
            { 2, 4, -2 }, { 0, 3, 8 }, { 1, 2, -1 }, { 3, 1, 5 }
        };

        tuc::sort_descending(data, [](SortTestItem const& item) { return std::get<1>(item); });

        EXPECT_EQ(data, data_sorted_by_index_1);

        {
            std::random_device random_device;
            std::mt19937 generator(random_device());
            std::shuffle(data.begin(), data.end(), generator);
        }

        auto const const_data = data;

        auto const copy_sorted_by_index_1 = tuc::sort_descending(const_data, [](SortTestItem const& item) { return std::get<1>(item); });

        EXPECT_EQ(copy_sorted_by_index_1, data_sorted_by_index_1);
    }

    TEST_F(FunctionalTest, FindsMinMaxElement) {
        std::vector<int> const values { 3, 2, 1 };
        auto const identity = [](auto value) { return value; };
        auto const min_element = tuc::min_element(values, identity);
        auto const max_element = tuc::max_element(values, identity);
        auto const minmax_element = tuc::minmax_element(values, identity);
        EXPECT_EQ(*min_element, 1);
        EXPECT_EQ(*max_element, 3);
        EXPECT_EQ(*minmax_element.first, 1);
        EXPECT_EQ(*minmax_element.second, 3);
    }

    TEST_F(FunctionalTest, EvaluatesLazily) {
        int counter = 0;
        auto const function = [&]() {
            return ++counter;
        };
        tuc::lazy_evaluator<int> const evaluator(function);
        EXPECT_EQ(evaluator.get_result(), 1);
        EXPECT_EQ(evaluator.get_result(), 1);
        EXPECT_EQ(counter, 1);
    }

    TEST_F(FunctionalTest, AppliesFunctionWithoutUnnecessaryCopyingData) {
        class Noncopyable {
        public:
            Noncopyable(int value)
                : value(value)
            {
                assert(value != -1);
            }
            int get_value() const {
                assert(value != -1);
                return value;
            }
            Noncopyable(Noncopyable&& that) {
                this->value = that.value;
                that.value = -1;
            }
        private:
            Noncopyable(Noncopyable const&) = delete; // non construction-copyable
            Noncopyable& operator=(Noncopyable const&) = delete; // non copyable

            int value = 0;
        };

        int evaluations = 0;

        auto const timesTwo = [&evaluations](Noncopyable const& noncopyable) {
            ++evaluations;
            return std::move(Noncopyable(noncopyable.get_value() * 2));
        };

        Noncopyable const one(1);

        typedef tuc::maybe_apply_function_without_unnecessary_copy_pattern<Noncopyable> pattern;

        {
            pattern const p(one, timesTwo, false);
            EXPECT_EQ(p.get().get_value(), 1);
            EXPECT_EQ(evaluations, 0);
        }

        {
            pattern const p(one, timesTwo, true);
            EXPECT_EQ(p.get().get_value(), 2);
            EXPECT_EQ(evaluations, 1);
        }
    }

    TEST_F(FunctionalTest, ActuallyReleasesMemory) {
        const size_t empty = 0;
        const size_t nonEmpty = 100;

        std::vector<int> v(nonEmpty);

        EXPECT_EQ(v.size(),     nonEmpty);
        EXPECT_EQ(v.capacity(), nonEmpty);

        v.clear();

        EXPECT_EQ(v.size(),     empty);
        EXPECT_EQ(v.capacity(), nonEmpty);

        tuc::release_memory(v);

        EXPECT_EQ(v.size(),     empty);
        EXPECT_EQ(v.capacity(), empty);
    }

}  // namespace
