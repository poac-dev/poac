#ifndef POAC_UTILITY_FILE_HPP
#define POAC_UTILITY_FILE_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>


namespace poac::utility::file {
    namespace fs = boost::filesystem;
    bool file_exists(const fs::path& filename) {
        boost::system::error_code error;
        if (const bool result = fs::exists(filename, error); !result || error)
            return false;
        else
            return true;
    }
} // end namespace
#endif // !POAC_UTILITY_FILE_HPP
