#pragma once

#ifdef WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace tuc
{ 
#ifdef WIN32
    namespace fs = std::filesystem;
#else
    namespace fs = std::experimental::filesystem::v1;
#endif

    inline void remove_empty_directories_recursively(fs::path const& path) {
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
