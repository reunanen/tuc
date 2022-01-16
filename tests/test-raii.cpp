struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/raii.hpp"
#include "picotest/picotest.h"
#include <stdio.h>
#include <fstream>

namespace {

    class ReleaseResourcesTest : public ::testing::Test {

    };

    TEST_F(ReleaseResourcesTest, ClosesAndRemovesFile) {
        char const* filename = "test.txt";

        // Let's open a file for writing, and then auto-close it
        {
            FILE* f = fopen(filename, "wb");
            auto const release = [f, filename]() {
                fclose(f);
                std::remove(filename);
            };
            tuc::raii::wrapper<FILE*, decltype(release)> w(f, release);
        }

        // Make sure the file was actually removed
        std::ifstream i(filename);
        EXPECT_FALSE(!!i);
    }

}  // namespace
