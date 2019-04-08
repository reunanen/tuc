#pragma once

#include <sstream>
#include <iomanip>

namespace tuc
{ 
    template <typename T>
    std::string to_string(T number, int precision, int min_precision = 1)
    {
        static_assert(std::is_floating_point<T>::value, "Floating-point type required"); // doesn't really make a lot of sense for other types
        int const actual_precision = std::max(precision + static_cast<int>(ceil(log10(1.0 / detail::round(number, precision))) - 1), min_precision);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(actual_precision) << number;
        return oss.str();
    }

    template <typename T>
    std::string to_percentage_string(T count, T total, int min_precision = 0)
    {
        static_assert(std::is_integral<T>::value, "Integral type required");
        int const precision = std::max(static_cast<int>(ceil(log10(total / 100.0))), min_precision);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << count * 100.0 / total;
        return oss.str();
    }

    namespace detail
    {
        template <typename T>
        T round(T value, int min_significant_digits)
        {
            if (value == 0.0)
            {
                return 0.0;
            }
            double const factor = std::pow(10.0, std::max(0.0, min_significant_digits - std::ceil(std::log10(std::abs(value)))));
            return std::round(value * factor) / factor;
        }
    }
}
