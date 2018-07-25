#ifndef POAC_IO_FILE_HPP
#define POAC_IO_FILE_HPP

#include <iostream>
#include <string>

//#include <boost/filesystem.hpp>


namespace poac::io::file {
//    namespace fs = boost::filesystem;
//    bool file_exists(const fs::path& filename) {
//        boost::system::error_code error;
//        if (const bool result = fs::exists(filename, error); !result || error)
//            return false;
//        else
//            return true;
//    }

    // Inspired by https://stackoverflow.com/questions/4891006/how-to-create-a-folder-in-the-home-directory
    std::string expand_user(std::string path) {
        if (!path.empty() && path[0] == '~') {
            assert(path.size() == 1 or path[1] == '/');  // or other error handling
            const char* home = std::getenv("HOME");
            if (home || ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            }
            else {
                const char *hdrive = std::getenv("HOMEDRIVE"),
                        *hpath  = std::getenv("HOMEPATH");
                assert(hdrive);  // or other error handling
                assert(hpath);
                path.replace(0, 1, std::string(hdrive) + hpath);
            }
        }
        return path;
    }
} // end namespace
#endif // !POAC_IO_FILE_HPP
