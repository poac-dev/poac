#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

// std
#include <filesystem>

namespace std::filesystem {
    inline namespace path_literals {
        inline std::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return std::filesystem::path(str);
        }
    }
}

namespace poac::io::path {
} // end namespace
#endif // !POAC_IO_PATH_HPP
