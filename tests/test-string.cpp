struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/string.hpp"
#include "picotest/picotest.h"

namespace {

    class StringTest : public ::testing::Test {
    };

    TEST_F(StringTest, FindsLeft) {
        EXPECT_EQ("ab", tuc::string::left("abc", 2));
        EXPECT_EQ(L"abc", tuc::wstring::left(L"abc", 4));
    }

    TEST_F(StringTest, FindsRight) {
        EXPECT_EQ(L"bc", tuc::wstring::right(L"abc", 2));
        EXPECT_EQ("abc", tuc::string::right("abc", 4));
    }

    TEST_F(StringTest, TellsIfStartsWith) {
        EXPECT_TRUE(tuc::string::starts_with("abc", "ab"));
        EXPECT_TRUE(tuc::string::starts_with("abc", "abc"));
        EXPECT_FALSE(tuc::wstring::starts_with(L"abc", L"abcd"));
    }

    TEST_F(StringTest, TellsIfEndsWith) {
        EXPECT_TRUE(tuc::wstring::ends_with(L"abc", L"bc"));
        EXPECT_TRUE(tuc::wstring::ends_with(L"abc", L"abc"));
        EXPECT_FALSE(tuc::string::ends_with("abc", "dabc"));
    }

    TEST_F(StringTest, TellsIfEqualCaseInsensitive) {
        EXPECT_TRUE(tuc::wstring::equal_case_insensitive(L"abc", L"abC"));
        EXPECT_FALSE(tuc::wstring::equal_case_insensitive(L"abc", L"äbC"));
        EXPECT_TRUE(tuc::string::equal_case_insensitive("abc", "abC"));
        EXPECT_FALSE(tuc::string::equal_case_insensitive("abc", "äbC"));
    }
}  // namespace
