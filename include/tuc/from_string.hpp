#pragma once

#include <string>
#include <limits>

#ifndef WIN32
#include <assert.h>
#endif // WIN32

namespace tuc
{ 
    template <typename T>
    T from_string(std::string const& string)
    {
        static_assert(std::is_floating_point<T>::value, "Floating-point type required"); // doesn't really make a lot of sense for other types

        if (string == std::to_string(std::numeric_limits<T>::quiet_NaN())) {
            return std::numeric_limits<T>::quiet_NaN();
        }

#if __cplusplus >= 201703L
        if constexpr (std::is_same<T, double>::value) {
            return std::stod(string);
        }
        if constexpr (std::is_same<T, float>::value) {
            return std::stof(string);
        }
#ifdef WIN32
        static_assert(false, "Unexpected type");
#else // WIN32
        assert(false); // Unexpected type
#endif // WIN32
#else
        return static_cast<T>(std::stod(string));
#endif
    }
}
