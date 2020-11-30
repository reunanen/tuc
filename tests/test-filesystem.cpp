struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/filesystem.hpp"
#include "picotest/picotest.h"
#include <stdio.h>
#include <fstream>

namespace {

    class FilesystemTest : public ::testing::Test {

    };

    TEST_F(FilesystemTest, RemovesEmptyDirectoriesRecursively) {
        tuc::fs::path test("filesystem-test-directory");
        tuc::fs::path nonEmpty = test / "foo" / "bar";
        tuc::fs::path empty = test / "foo" / "baz" / "zap";
        
        tuc::fs::path testFile = nonEmpty / "test.txt";

        tuc::fs::create_directories(nonEmpty);
        tuc::fs::create_directories(empty);

        {
            std::ofstream out(testFile);
            out << "test file";
        }

        tuc::remove_empty_directories_recursively(test);

        EXPECT_TRUE(tuc::fs::exists(test));
        EXPECT_TRUE(tuc::fs::exists(nonEmpty));
        EXPECT_FALSE(tuc::fs::exists(empty));
        EXPECT_TRUE(tuc::fs::exists(testFile));

        tuc::fs::remove(testFile);

        tuc::remove_empty_directories_recursively(test);

        EXPECT_FALSE(tuc::fs::exists(testFile));
        EXPECT_FALSE(tuc::fs::exists(nonEmpty));
        EXPECT_FALSE(tuc::fs::exists(test));
    }

}  // namespace
