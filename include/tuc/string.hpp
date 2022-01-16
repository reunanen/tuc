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
            auto const equal = [](auto const& c1, auto const& c2) {
                return c1 == c2 || std::tolower(c1) == std::tolower(c2);
            };
            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), equal);
        }

        // adapted from here: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
        template <typename String>
        size_t replace_all(String& in_out, String const& before, String const& after)
        {
            size_t start_pos = 0, counter = 0;
            while ((start_pos = in_out.find(before, start_pos)) != std::string::npos) {
                in_out.replace(start_pos, before.length(), after);
                start_pos += after.length();
                ++counter;
            }
            return counter;
        }
    }

    // Convenience wrappers for std::string
    namespace string
    {
        inline std::string left(std::string const& input, size_t n)
        {
            return generic_string::left(input, n);
        }

        inline std::string right(std::string const& input, size_t n)
        {
            return generic_string::right(input, n);
        }

        inline bool starts_with(std::string const& input, std::string const& candidate)
        {
            return generic_string::starts_with(input, candidate);
        }

        inline bool ends_with(std::string const& input, std::string const& candidate)
        {
            return generic_string::ends_with(input, candidate);
        }

        inline bool equal_case_insensitive(std::string const& lhs, std::string const& rhs)
        {
            return generic_string::equal_case_insensitive(lhs, rhs);
        }

        inline size_t replace_all(std::string& in_out, std::string const& before, std::string const& after)
        {
            return generic_string::replace_all(in_out, before, after);
        }
    }

    // Convenience wrappers for std::wstring
    namespace wstring
    {
        inline std::wstring left(std::wstring const& input, size_t n)
        {
            return generic_string::left(input, n);
        }

        inline std::wstring right(std::wstring const& input, size_t n)
        {
            return generic_string::right(input, n);
        }

        inline bool starts_with(std::wstring const& input, std::wstring const& candidate)
        {
            return generic_string::starts_with(input, candidate);
        }

        inline bool ends_with(std::wstring const& input, std::wstring const& candidate)
        {
            return generic_string::ends_with(input, candidate);
        }

        inline bool equal_case_insensitive(std::wstring const& lhs, std::wstring const& rhs)
        {
            return generic_string::equal_case_insensitive(lhs, rhs);
        }

        inline size_t replace_all(std::wstring& in_out, std::wstring const& before, std::wstring const& after)
        {
            return generic_string::replace_all(in_out, before, after);
        }
    }
}
