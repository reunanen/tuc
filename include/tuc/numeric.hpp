#pragma once

#include <type_traits>
#include <algorithm> // std::min, std::max
#include <assert.h>

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

    // Provide a simple clamp function that can be used with pre-C++17 compilers
    template <typename T>
    T clamp(T const& value, T const& low_limit, T const& high_limit) {
        assert(low_limit <= high_limit);
        return std::max(low_limit, std::min(high_limit, value));
    }

    namespace smoothstep_detail {
        template <typename T>
        T prepare(T const& left_edge, T const& right_edge, T const& v) {
            assert(right_edge > left_edge);
            return tuc::clamp<T>(
                (v - left_edge) / (right_edge - left_edge),
                0, 1
            );
        }
    }

    template <typename T>
    T smoothstep(T const& left_edge, T const& right_edge, T const& v) {
        T const x = smoothstep_detail::prepare(left_edge, right_edge, v);
        return x * x * (3 - 2 * x);
    }

    template <typename T>
    T smootherstep(T const& left_edge, T const& right_edge, T const& v) {
        T const x = smoothstep_detail::prepare(left_edge, right_edge, v);
        return x * x * x * (x * (x * 6 - 15) + 10);
    }
}
