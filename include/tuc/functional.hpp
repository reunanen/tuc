#pragma once

#include <vector>

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

    namespace detail {
        template <typename NotVector> void reserve(NotVector& not_vector, size_t new_capacity) {
            // By default, do nothing (not std::vector)
        }
        template <typename VectorElement> void reserve(std::vector<VectorElement>& vector, size_t new_capacity) {
            vector.reserve(new_capacity);
        }
    }

}