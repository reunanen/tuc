#pragma once

#include <vector>
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
    Output filter(Input const& input, AcceptFunction function, size_t expected_size = -1)
    {
        if (expected_size == -1) {
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
        auto const compare = [to_value](auto const& lhs, auto const& rhs) {
            return to_value(lhs) < to_value(rhs);
        };

        std::sort(input_and_output.begin(), input_and_output.end(), compare);
    }

    template <typename InputAndOutput, typename ToValue>
    InputAndOutput sort_ascending(InputAndOutput const& input, ToValue to_value)
    {
        auto const compare = [to_value](auto const& lhs, auto const& rhs) {
            return to_value(lhs) < to_value(rhs);
        };

        auto output = input;

        std::sort(output.begin(), output.end(), compare);

        return output;
    }
}