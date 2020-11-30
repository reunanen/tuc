#pragma once

#ifdef WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace tuc
{ 
    namespace fs = std::experimental::filesystem::v1;

    void remove_empty_directories_recursively(const fs::path& path) {
        for (fs::directory_iterator i(path), end; i != end; ++i) {
            if (fs::is_directory(i->path())) {
                remove_empty_directories_recursively(i->path());
            }
        }
        if (fs::is_empty(path)) {
            fs::remove(path);
        }
    }
}
