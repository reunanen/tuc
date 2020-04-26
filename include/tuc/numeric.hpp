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

    template <typename T>
    T divide_rounding_to_closest(T numerator, T denominator)
    {
        static_assert(std::is_integral<T>::value, "Integral type required");
        return (numerator < 0) ^ (denominator < 0)
            ? (numerator - denominator / 2) / denominator
            : (numerator + denominator / 2) / denominator;
    }

    template <typename T>
    T divide_rounding_down(T numerator, T denominator)
    {
        static_assert(std::is_integral<T>::value, "Integral type required");
        return (numerator < 0) ^ (denominator < 0)
            ? (numerator - denominator + 1) / denominator
            : numerator / denominator;
    }

    template <typename T>
    T lerp(T const& v0, T const& v1, double t) {
        return (1.0 - t) * v0 + t * v1;
    }
}
