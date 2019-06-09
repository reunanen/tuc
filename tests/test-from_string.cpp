struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/from_string.hpp"
#include "picotest/picotest.h"

namespace {

    class FromStringTest : public ::testing::Test {
    };

    TEST_F(FromStringTest, ReadsNumber) {
        EXPECT_EQ(1.0, tuc::from_string<double>("1.0"));
    }

    TEST_F(FromStringTest, ReadsNaN) {
        std::string string = std::to_string(std::numeric_limits<double>::quiet_NaN());
        EXPECT_TRUE(std::isnan(tuc::from_string<double>(string)));
    }
}  // namespace
