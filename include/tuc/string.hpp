#pragma once

#include <string>
#include <cctype> // std::tolower
#include <algorithm>

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
            return input.substr(length - (std::min)(length, n));
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

        template <typename String>
        bool equal_case_insensitive(String const& lhs, String const& rhs)
        {
            if (lhs.size() != rhs.size()) {
                return false;
            }
            const auto equal = [](auto const& c1, auto const& c2) {
                return c1 == c2 || std::tolower(c1) == std::tolower(c2);
            };
            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), equal);
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

        bool equal_case_insensitive(std::string const& lhs, std::string const& rhs)
        {
            return generic_string::equal_case_insensitive(lhs, rhs);
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

        bool equal_case_insensitive(std::wstring const& lhs, std::wstring const& rhs)
        {
            return generic_string::equal_case_insensitive(lhs, rhs);
        }
    }
}
