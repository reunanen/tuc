#pragma once

// To be included only via tuc/to_string.hpp

namespace tuc
{ 
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
