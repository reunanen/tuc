#pragma once

#include <type_traits>

namespace tuc
{ 
    template <typename T>
    T divide_rounding_up(T numerator, T denominator)
    {
        static_assert(std::is_integral<T>::value, "Integral type required");
        return numerator == 0
            ? 0
            : 1 + ((numerator - 1) / denominator);
    }

    // Trivial, but provided for completeness
    template <typename T>
    T divide_rounding_down(T numerator, T denominator)
    {
        static_assert(std::is_integral<T>::value, "Integral type required");
        return numerator / denominator;
    }
}
