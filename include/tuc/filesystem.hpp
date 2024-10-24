#pragma once

#if defined(WIN32) || defined(__APPLE__)
// no need to use "experimental" filesystem
#else
#define TUC_USE_EXPERIMENTAL_FILESYSTEM
#endif

#ifdef TUC_USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

namespace tuc
{ 
#ifdef TUC_USE_EXPERIMENTAL_FILESYSTEM
    namespace fs = std::experimental::filesystem::v1;
#else
    namespace fs = std::filesystem;
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
