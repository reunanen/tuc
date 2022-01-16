#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <iterator>
#include "functional_detail.hpp"

namespace tuc
{    
    template <typename Output, typename Input, typename MapFunction>
    Output map(Input const& input, MapFunction function)
    {
        Output output;
        detail::reserve(output, input.size()); // Perhaps std::transform is smart enough that this is not really needed for performance.
        std::transform(input.begin(), input.end(), std::back_inserter(output), function);
        return output;
    }

    template <typename Output, typename Input, typename AcceptFunction>
    Output filter(Input const& input, AcceptFunction function, size_t expected_size = (std::numeric_limits<size_t>::max)())
    {
        if (expected_size == (std::numeric_limits<size_t>::max)()) {
            expected_size = input.size();
        }
        Output output;
        detail::reserve(output, expected_size);
        std::copy_if(input.begin(), input.end(), std::back_inserter(output), function);
        return output;
    }

    template <typename InputAndOutput, typename AcceptFunction>
    void remove_if(InputAndOutput& input_and_output, AcceptFunction function)
    {
        input_and_output.erase(
            std::remove_if(
                input_and_output.begin(),
                input_and_output.end(),
                function
            ),
            input_and_output.end()
        );
    }

    template <typename InputAndOutput, typename ToValue>
    void sort_ascending(InputAndOutput& input_and_output, ToValue to_value)
    {
        std::sort(input_and_output.begin(), input_and_output.end(), detail::get_compare_function(to_value));
    }

    template <typename InputAndOutput, typename ToValue>
    InputAndOutput sort_ascending(InputAndOutput const& input, ToValue to_value)
    {
        auto output = input;

        std::sort(output.begin(), output.end(), detail::get_compare_function(to_value));

        return output;
    }

    template <typename InputAndOutput, typename ToValue>
    void sort_descending(InputAndOutput& input_and_output, ToValue to_value)
    {
        std::sort(input_and_output.rbegin(), input_and_output.rend(), detail::get_compare_function(to_value));
    }

    template <typename InputAndOutput, typename ToValue>
    InputAndOutput sort_descending(InputAndOutput const& input, ToValue to_value)
    {
        auto output = input;

        std::sort(output.rbegin(), output.rend(), detail::get_compare_function(to_value));

        return output;
    }

    template <typename Input, typename ToValue>
    auto min_element(Input const& input, ToValue to_value)
    {
        return std::min_element(input.begin(), input.end(), detail::get_compare_function(to_value));
    }

    template <typename Input, typename ToValue>
    auto max_element(Input const& input, ToValue to_value)
    {
        return std::max_element(input.begin(), input.end(), detail::get_compare_function(to_value));
    }

    template <typename Input, typename ToValue>
    auto minmax_element(Input const& input, ToValue to_value)
    {
        return std::minmax_element(input.begin(), input.end(), detail::get_compare_function(to_value));
    }

    template <typename Result>
    class lazy_evaluator
    {
    public:
        lazy_evaluator(std::function<Result()> function)
            : function(function)
        {}

        Result const& get_result() const {
            if (!result) {
                result = std::make_unique<Result>(function());
            }
            return *result;
        }

    private:
        std::function<Result()> const function;
        std::unique_ptr<Result> mutable result;
    };

    template <typename T>
    class maybe_apply_function_without_unnecessary_copy_pattern
    {
    public:
        maybe_apply_function_without_unnecessary_copy_pattern(T const& input, std::function<T(T const&)> function, bool apply)
            : input(input)
            , function_result(
                apply
                    ? std::make_unique<T>(function(input))
                    : std::unique_ptr<T>()
            )
        {}

        T const& get() const {
            return function_result
                ? *function_result
                : input;
        }

    private:
        T const& input;
        std::unique_ptr<T> const function_result;
    };
}