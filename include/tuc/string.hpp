#pragma once

#include <string>

namespace tuc
{ 
    namespace generic_string
    {
        template <typename String>
        String left(String const& input, size_t n)
        {
            return input.substr(0, n);
        }

        template <typename String>
        String right(String const& input, size_t n)
        {
            auto const length = input.length();
            return input.substr(length - std::min(length, n));
        }

        template <typename String>
        bool starts_with(String const& input, String const& candidate)
        {
            auto const candidate_length = candidate.length();
            return candidate_length <= input.length()
                && candidate == left(input, candidate_length);
        }

        template <typename String>
        bool ends_with(String const& input, String const& candidate)
        {
            auto const candidate_length = candidate.length();
            return candidate_length <= input.length()
                && candidate == right(input, candidate_length);
        }
    }

    // Convenience wrappers for std::string
    namespace string
    {
        std::string left(std::string const& input, size_t n)
        {
            return generic_string::left(input, n);
        }

        std::string right(std::string const& input, size_t n)
        {
            return generic_string::right(input, n);
        }

        bool starts_with(std::string const& input, std::string const& candidate)
        {
            return generic_string::starts_with(input, candidate);
        }

        bool ends_with(std::string const& input, std::string const& candidate)
        {
            return generic_string::ends_with(input, candidate);
        }
    }

    // Convenience wrappers for std::wstring
    namespace wstring
    {
        std::wstring left(std::wstring const& input, size_t n)
        {
            return generic_string::left(input, n);
        }

        std::wstring right(std::wstring const& input, size_t n)
        {
            return generic_string::right(input, n);
        }

        bool starts_with(std::wstring const& input, std::wstring const& candidate)
        {
            return generic_string::starts_with(input, candidate);
        }

        bool ends_with(std::wstring const& input, std::wstring const& candidate)
        {
            return generic_string::ends_with(input, candidate);
        }
    }
}
