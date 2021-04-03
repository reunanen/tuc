#pragma once

#include <type_traits>
#include <algorithm> // std::min, std::max
#include <numeric>
#include <cmath>
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

    // A convenience wrapper for combined rounding and typecasting
    template <typename Output = int, typename Input>
    Output round(Input input)
    {
        static_assert(std::is_integral<Output>::value, "Integral output type required - use std::round for other purposes");
        auto const rounded = std::round(input);
        if (rounded < std::numeric_limits<Output>::lowest() || rounded > (std::numeric_limits<Output>::max)()) {
            throw std::runtime_error(
                "Numeric overflow in tuc::round (input = " + std::to_string(input) + "," +
                " valid range = [" + std::to_string(std::numeric_limits<Output>::lowest()) +
                ", " + std::to_string((std::numeric_limits<Output>::max)()) + "])"
            );
        }
        return static_cast<Output>(rounded);
    }

    template <typename T>
    T lerp(T const& v0, T const& v1, double t) {
        return (1.0 - t) * v0 + t * v1;
    }

    // Provide a simple clamp function that can be used with pre-C++17 compilers
    template <typename T>
    T clamp(T const& value, T const& low_limit, T const& high_limit) {
        assert(low_limit <= high_limit);
        return (std::max)(low_limit, (std::min)(high_limit, value));
    }

    template <typename T>
    class unambiguous_clamp {
    public:
        unambiguous_clamp& low(T const& low_) {
            this->low_limit = low_;
#ifndef NDEBUG
            assert(!low_limit_defined);
            low_limit_defined = true;
#endif // NDEBUG
            return *this;
        }

        unambiguous_clamp& high(T const& high_) {
            this->high_limit = high_;
#ifndef NDEBUG
            assert(!high_limit_defined);
            high_limit_defined = true;
#endif // NDEBUG
            return *this;
        }

        unambiguous_clamp& value(T const& value_) {
            v = value_;
#ifndef NDEBUG
            assert(!value_defined);
            value_defined = true;
#endif // NDEBUG
            return *this;
        }

        operator T() const {
            assert(low_limit_defined);
            assert(high_limit_defined);
            assert(value_defined);
            return tuc::clamp(v, low_limit, high_limit);
        }

    private:
        T v;
        T low_limit;
        T high_limit;
#ifndef NDEBUG
        bool value_defined = false;
        bool low_limit_defined = false;
        bool high_limit_defined = false;
#endif // NDEBUG
    };

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

    template <typename T>
    int sign(T const& value) {
        T constexpr zero(0);
        // idea taken from https://stackoverflow.com/a/4609795/19254
        return (zero < value) - (value < zero);
    }

    template <typename Iterator, typename T = double>
    T arithmetic_mean(Iterator begin, Iterator end) {
        return std::accumulate(begin, end, static_cast<T>(0)) / std::distance(begin, end);
    }

    template <typename Iterator, typename T = double>
    T geometric_mean(Iterator begin, Iterator end) {
        return std::pow(std::accumulate(begin, end, static_cast<T>(1), std::multiplies<T>()), 1.0 / std::distance(begin, end));
    }

    template <typename Iterator, typename T = double, typename P = double>
    T power_mean(Iterator begin, Iterator end, P p) {
        if (p == 0) {
            return tuc::geometric_mean(begin, end);
        }

        auto const accumulate = [p](T a, T b) {
            return a + pow(b, p);
        };
        auto const result = std::pow(std::accumulate(begin, end, static_cast<T>(0), accumulate) / std::distance(begin, end), 1.0 / p);
        if (std::isinf(result)) {
            return *std::max_element(begin, end);
        }
        return result;
    }

}
